#include "pring.h"
#include <stddef.h>

void init_timer() {
    uint32_t divisor = PIT_FREQUENCY / TIMER_HZ;
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
    boot_time = timer_ticks;
}

void* malloc(size_t size) {
    if (heap_start + size > heap_end) {
        return NULL;
    }
    void* ptr = (void*)heap_start;
    heap_start += size;
    return ptr;
}

void free(void* ptr) {
}

void* calloc(size_t num, size_t size) {
    void* ptr = malloc(num * size);
    if (ptr) {
        memset(ptr, 0, num * size);
    }
    return ptr;
}

void* realloc(void* ptr, size_t new_size) {
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    void* new_ptr = malloc(new_size);
    if (new_ptr && ptr) {
        memcpy(new_ptr, ptr, new_size);
        free(ptr);
    }
    return new_ptr;
}

#define ETH_TYPE_IP 0x0800
#define ETH_TYPE_ARP 0x0806
#define ETH_ALEN 6

struct eth_header {
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint16_t type;
} __attribute__((packed));

#define IP_PROTO_TCP 6
#define IP_PROTO_UDP 17

struct ip_header {
    uint8_t ver_ihl;
    uint8_t tos;
    uint16_t total_len;
    uint16_t id;
    uint16_t flags_frag;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
} __attribute__((packed));

struct udp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed));

#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68
#define DHCP_MAGIC_COOKIE 0x63825363
#define DHCP_DISCOVER 1
#define DHCP_OFFER 2
#define DHCP_REQUEST 3
#define DHCP_ACK 5

struct dhcp_header {
    uint8_t op;
    uint8_t htype;
    uint8_t hlen;
    uint8_t hops;
    uint32_t xid;
    uint16_t secs;
    uint16_t flags;
    uint32_t ciaddr;
    uint32_t yiaddr;
    uint32_t siaddr;
    uint32_t giaddr;
    uint8_t chaddr[16];
    uint8_t sname[64];
    uint8_t file[128];
    uint32_t magic;
    uint8_t options[308];
} __attribute__((packed));

struct net_device {
    char name[16];
    uint8_t mac[6];
    uint32_t ip;
    uint32_t gateway;
    uint32_t netmask;
    uint32_t dns;
    bool connected;
    bool dhcp_enabled;
};

void network_init();
bool detect_network_card();
void get_mac_address(uint8_t* mac);
void send_ethernet_frame(uint8_t* dst_mac, uint16_t type, uint8_t* data, uint32_t len);
void handle_ethernet_frame(uint8_t* frame, uint32_t len);
void dhcp_discover();
void dhcp_request(uint32_t offered_ip);
void handle_dhcp_packet(struct dhcp_header* dhcp, uint32_t len);
void process_network_packets();
uint16_t checksum(void* data, uint32_t len);
uint32_t parse_ip(const char* ip_str);
void ip_to_str(uint32_t ip, char* buf);

void execute_dhcpcd(char* interface);
void execute_ifconfig();
void execute_ping(char* host);

extern struct net_device net_dev;
