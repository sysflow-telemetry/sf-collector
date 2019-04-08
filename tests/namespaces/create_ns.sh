#!/bin/bash
sudo umount -l /tmp/ns; rm -r /tmp/ns
mkdir -p /tmp/ns
sudo mount --bind --make-private /tmp/ns /tmp/ns
touch /tmp/ns/pid
sudo unshare --pid=/tmp/ns/pid -f
