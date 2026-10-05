---
layout: post
title: "Set up VPN server using docker-openvpn"
tags: [OpenVPN, Docker]
categories: network
date: 2023-05-03 12:00:00 -0700
---

## Introduction

A Virtual Private Network (VPN) is a tool that allows you to securely connect to the internet, encrypting all of your internet traffic and hiding your IP address from prying eyes. While there are plenty of VPN services available, setting up your own VPN server can offer several benefits, including increased privacy and control over your data. In this post, we'll show you how to use docker-openvpn to set up your own VPN server quickly and easily.
## Pre-requisites

To set up docker-openvpn, you need to install Docker first.
- Download and install Docker from the official website. 
This is pretty much all you need for your machine. Just remember to prevent it from sleep if you are using your laptop.
## Setting up docker-openvpn

```shell
OVPN_DATA="ovpn-data-example"
OVPN_FILE="example"

# Initialize the container that will hold the configuration files and certificates.
docker volume create --name $OVPN_DATA
docker run -v $OVPN_DATA:/etc/openvpn --rm kylemanna/openvpn ovpn_genconfig -u udp://x.x.x.x:51194
docker run -v $OVPN_DATA:/etc/openvpn --rm -it kylemanna/openvpn ovpn_initpki

# Start OpenVPN server process
docker run -v $OVPN_DATA:/etc/openvpn -d -p 51194:1194/udp --cap-add=NET_ADMIN kylemanna/openvpn

# Generate a client certificate without a passphrase
docker run -v $OVPN_DATA:/etc/openvpn --rm -it kylemanna/openvpn easyrsa build-client-full $OVPN_FILE nopass

# Retrieve the client configuration with embedded certificates
docker run -v $OVPN_DATA:/etc/openvpn --rm kylemanna/openvpn ovpn_getclient $OVPN_FILE > $OVPN_FILE.ovpn
```
To run this script, you need to figure out your public IP address first. I recommend to just visit [https://ifconfig.me/](https://ifconfig.me/).
The reason I picked the port 51194 is because the default port of openvpn 1194 would have high possibility to be blocked when the network client side is using doesn’t allow VPN connection. Sometimes, even a random port could have possibility to be blocked as well. Just change to another unused port should fix the problem. 
## Configuring the VPN client

On iOS, you can just simply download OpenVPN App from App Store. And then load the client config file(`*.ovpn`) you just exported.
On macOS, you need third-party open-source software to make it work. You can install Tunnelblick.
Load the client config file and you are good to go.
## Attention

After you have finished the configuration setup, you also need to ensure the port you are using is reachable.
If you are using your own machine and under a home network with router. You most likely need to setup the Port Forwarding in your router setting. Just remember the protocol we are using above is UDP.
If you are using virtual instances, like EC2 for example, you might need to configure the Security Group to allow the port.
## Improvements

There are lots of space to improve this network.
1. Improve the speed for the VPN client
2. Multiple client configurations
3. Automatic switch ports periodically
## Reference

[https://github.com/kylemanna/docker-openvpn](https://github.com/kylemanna/docker-openvpn)
