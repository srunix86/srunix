struct net_device net_dev;

void network_init() {
    memset(&net_dev, 0, sizeof(net_dev));
    strcpy(net_dev.name, "eth0");
    
    uint8_t fake_mac[] = {0x52, 0x54, 0x00, 0x12, 0x34, 0x56};
    memcpy(net_dev.mac, fake_mac, 6);
    
    net_dev.dhcp_enabled = true;
}

void dhcp_discover() {
    net_dev.ip = 0xC0A80064;
    net_dev.gateway = 0xC0A80001;
    net_dev.netmask = 0xFFFFFF00;
    net_dev.dns = 0x08080808;
    net_dev.connected = true;
}

void ip_to_str(uint32_t ip, char* buf) {
    char temp[4];
    char* p = buf;
    
    int_to_str((ip >> 24) & 0xFF, p);
    p += strlen(p);
    *p++ = '.';
    int_to_str((ip >> 16) & 0xFF, p);
    p += strlen(p);
    *p++ = '.';
    int_to_str((ip >> 8) & 0xFF, p);
    p += strlen(p);
    *p++ = '.';
    int_to_str(ip & 0xFF, p);
}

void execute_dhcpcd(char* interface) {
    if (interface == NULL) {
        terminal_writestring("Usage: dhcpcd <interface>\n");
        return;
    }
    
    if (strcmp(interface, "eth0") != 0) {
        terminal_printf("Interface %s not found\n", interface);
        return;
    }
    
    terminal_printf("Starting DHCP on %s...\n", interface);
    dhcp_discover();
    
    if (net_dev.connected) {
        char ip_str[16];
        ip_to_str(net_dev.ip, ip_str);
        terminal_printf("Interface %s configured: %s\n", interface, ip_str);
    } else {
        terminal_writestring("DHCP failed\n");
    }
}

void execute_ifconfig() {
    if (!net_dev.dhcp_enabled) {
        terminal_writestring("No network interfaces found\n");
        return;
    }
    
    char mac_str[18];
    char ip_str[16] = "Not configured";
    char gateway_str[16] = "Not configured";
    char netmask_str[16] = "Not configured";
    
    char* p = mac_str;
    for (int i = 0; i < 6; i++) {
        uint8_t byte = net_dev.mac[i];
        uint8_t high = byte >> 4;
        uint8_t low = byte & 0x0F;
        
        *p++ = (high < 10) ? '0' + high : 'a' + (high - 10);
        *p++ = (low < 10) ? '0' + low : 'a' + (low - 10);
        if (i < 5) *p++ = ':';
    }
    *p = '\0';
    
    if (net_dev.connected) {
        ip_to_str(net_dev.ip, ip_str);
        ip_to_str(net_dev.gateway, gateway_str);
        ip_to_str(net_dev.netmask, netmask_str);
    }
    
    terminal_printf("eth0:\n");
    terminal_printf("  MAC:     %s\n", mac_str);
    terminal_printf("  IP:      %s\n", ip_str);
    terminal_printf("  Gateway: %s\n", gateway_str);
    terminal_printf("  Netmask: %s\n", netmask_str);
    terminal_printf("  Status:  %s\n", net_dev.connected ? "Connected" : "Disconnected");
}

void execute_ping(char* host) {
    if (!net_dev.connected) {
        terminal_writestring("Network not configured. Run 'dhcpcd eth0' first\n");
        return;
    }
    
    if (host == NULL) {
        terminal_writestring("Usage: ping <host>\n");
        return;
    }
    
    terminal_printf("PING simulator %s\n", host);
    terminal_writestring("64 bytes from 192.168.0.1: icmp_seq=1 ttl=64 time=1.23 ms\n");
    terminal_writestring("64 bytes from 192.168.0.1: icmp_seq=2 ttl=64 time=1.45 ms\n");
    terminal_writestring("64 bytes from 192.168.0.1: icmp_seq=3 ttl=64 time=1.67 ms\n");
    terminal_writestring("");
    terminal_writestring("3 packets transmitted, 3 received, 0% packet loss\n");
}

void vga_save_current_font(uint8_t* buffer) {
    outb(0x3C4, 0x04); 
    outb(0x3C4, 0x02); 
    outb(0x3CE, 0x04); 
    outb(0x3CE, 0x02); 
    outb(0x3CE, 0x05); 
    outb(0x3CE, 0x00); 
    outb(0x3CE, 0x06); 
    outb(0x3CE, 0x00); 

    volatile uint8_t* font_mem = (volatile uint8_t*)0xA0000;
    for (int i = 0; i < VGA_FONT_SIZE; i++) {
        buffer[i] = font_mem[i];
    }

    outb(0x3C4, 0x03); 
    outb(0x3C4, 0x00);
    outb(0x3CE, 0x04); 
    outb(0x3CE, 0x00);
    outb(0x3CE, 0x05);
    outb(0x3CE, 0x10);
    outb(0x3CE, 0x06);
    outb(0x3CE, 0x0E);
}
