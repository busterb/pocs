/*
 * A low-CPU mechanism for doing line-rate packet capture with vanilla libpcap
 * using adaptive sleep to avoid too many syscalls. Works best with OSes that
 * have a shared-memory ring buffer mechanism, like Linux.
 *
 * This will process up to PCAP_BUFFER_PKTS packets at a time without a context
 * switch, at the expense of increased latency on a per-packet basis. With
 * these default parameters, it should be able to run at 10 gigabit rates with
 * with normal traffic only a few percent CPU utilization. I haven't run it
 * with a real network testing tool to validate max speed.
 *
 * gcc fastpcap.c  -lpcap -lpthread -Wall -o fastpcap
 *
 */
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/time.h>
#include <pcap/pcap.h>
#include <pthread.h>
#include <unistd.h>

#define PCAP_MAX_PKT_BATCH 100000
#define PCAP_ACTIVE_PKT_SLEEP_US 1000
#define PCAP_INACTIVE_PKT_SLEEP_US 100000
#define PCAP_SNAP_LEN 128
#define PCAP_BUFFER_PKTS 16384
#define PCAP_BUFFER_SIZE (PCAP_SNAP_LEN * PCAP_BUFFER_PKTS)
#define PCAP_TIMEOUT_MS 10

struct stats {
	uint64_t rx_packets;
	uint64_t rx_bytes;
};

static void intf_rx_cb(u_char *arg, const struct pcap_pkthdr *h, const u_char *buf)
{
	struct stats *stats = (void *)arg;
	stats->rx_bytes += h->len;
	stats->rx_packets++;
}

uint64_t time_us(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000000ULL) + tv.tv_usec;
}

void *stats_printer(void *arg)
{
	struct stats *stats = arg, prev_stats = *stats;
	while (1) {
		printf("rx bytes: %"PRIu64"\nrx_packets: %"PRIu64"\n",
				stats->rx_bytes   - prev_stats.rx_bytes,
				stats->rx_packets - prev_stats.rx_packets);
		prev_stats = *stats;
		usleep(1000000);
	}
	return NULL;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <intf>\n", argv[0]);
		return 1;
	}

	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *pcap = pcap_open_live(argv[1], PCAP_SNAP_LEN, 1, PCAP_TIMEOUT_MS, errbuf);
	if (pcap == NULL) {
		fprintf(stderr, "Error: %s\n", errbuf);
		return 1;
	}

	pcap_set_buffer_size(pcap, PCAP_BUFFER_SIZE);

	struct stats stats = {0};
	pthread_t stats_thread;
	pthread_create(&stats_thread, NULL, stats_printer, &stats);

	uint64_t start_us;
	int rc = 0;

	do {
		/*
		 * Measure the time spent processing packets
		 */
		start_us = time_us();

		rc = pcap_dispatch(pcap, PCAP_MAX_PKT_BATCH, intf_rx_cb, (u_char *)&stats);

		/*
		 * Determine if we should add additional sleep calls to get better
		 * batching behavior
		 */
		uint64_t processing_us = time_us() - start_us;
		if (rc > 0 && processing_us < PCAP_ACTIVE_PKT_SLEEP_US) {
			usleep(PCAP_ACTIVE_PKT_SLEEP_US - processing_us);
		} else if (rc == 0) {
			usleep(PCAP_INACTIVE_PKT_SLEEP_US);
		}
	} while (rc >= 0);
}
