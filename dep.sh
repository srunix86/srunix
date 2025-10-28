#!/bin/bash

check_root() {
    if [ "$EUID" -ne 0 ]; then
        echo "Error: This script must be run as root. Use sudo or doas."
        exit 1
    fi
}

detect_os() {
    if [ -f /etc/os-release ]; then
        source /etc/os-release
        echo "$ID"
    else
        echo "unknown"
    fi
}

install_packages() {
    local os_id=$1
    local packages="nasm gcc binutils grub2 xorriso make mtools qemu-system"
    
    case $os_id in
        ubuntu|debian|linuxmint)
            apt update
            apt install -y $packages
            ;;
        fedora|rhel|centos)
            dnf install -y $packages
            ;;
        arch|manjaro|endeavouros)
            pacman -Sy --noconfirm $packages
            ;;
        opensuse*|suse)
            zypper install -y $packages
            ;;
        alpine)
            apk add $packages
            ;;
        gentoo)
            emerge --ask n $packages
            ;;
        slackware)
            slackpkg install $packages
            ;;
        void)
            xbps-install -S $packages
            ;;
        solus)
            eopkg install $packages
            ;;
        crux)
            prt-get depinst $packages
            ;;
        *)
            echo "Unsupported operating system: $os_id"
            exit 1
            ;;
    esac
}

main() {
    check_root
    local os_id=$(detect_os)
    install_packages "$os_id"
}

main "$@"
