from bluepy.btle import UUID, Peripheral

config = {
    "DEVICE_MAC": "d8:4c:90:d5:48:f0"
}

def print_profile(profile):
    print("Profile")
    for s in profile:
        print("  Service")
        print("    %s" % (s["Service"]))
        print("  Characteristics")
        for c in s["Characteristics"]:
            print("    %s: %s, readable: %r" % (str(c), c.propertiesToString(), c.supportsRead()))
            print("      des:")
            for d in c.getDescriptors():
                print("        handle=%s, uuid=%s, value=%s" % (d.handle, d.uuid, d.read()))
        print("  Descriptor")
        for d in s["Descriptor"]:
            print("    handle=%s, uuid=%s, value=%s" % (d.handle, d.uuid, d.read()))
        print("\n")

def main():
    print("Connecting...")
    dev = Peripheral(config["DEVICE_MAC"], "public")
    try:
        print("Conneted")
        services = dev.getServices()
        profile = []
        for service in services:
            print(str(service))
            profile.append({
                "Service": service,
                "Characteristics": service.getCharacteristics(),
                "Descriptor": service.getDescriptors(forUUID=0x2902)
            })
        
        print_profile(profile)

        print("Changing notify...")
        for d in profile[2]["Descriptor"]:
            print(d.write(val=b"\x01\x00", withResponse=True))

        print_profile(profile)

    finally:
        dev.disconnect()

if __name__ == "__main__":
    main()

# from bluepy.btle import Peripheral, UUID
# from bluepy.btle import Scanner, DefaultDelegate


# class ScanDelegate(DefaultDelegate):
#     def __init__(self):
#         DefaultDelegate.__init__(self)

#     def handleDiscovery(self, dev, isNewDev, isNewData):
#         if isNewDev:
#             print("Discovered device", dev.addr)
#         elif isNewData:
#             print("Received new data from", dev.addr)


# # ---- add ----
# class myDelegate(DefaultDelegate):
#     def __init__(self):
#         DefaultDelegate.__init__(self)

#     def handleNotification(self, cHandle, data):
#         print("A notification was received: %s" % data)
# # -------------


# # scanner = Scanner().withDelegate(ScanDelegate())
# # devices = scanner.scan(10.0)
# # n = 0
# # for dev in devices:
# #     print("%d: Device %s (%s), RSSI=%d dB" %
# #           (n, dev.addr, dev.addrType, dev.rssi))
# #     n += 1
# #     for (adtype, desc, value) in dev.getScanData():
# #         print("  %s = %s" % (desc, value))

# # number = input('Enter your device number: ')
# # number = int(number)
# # print('Device', number)
# # # print(type(devices))
# # print(list(devices)[number].addr)

# print("Connecting...")
# dev = Peripheral("a8:9c:ed:a0:5e:fb", 'public')
# # ----- add -----
# dev.setDelegate(myDelegate())

# print("Services...")
# for svc in dev.services:
#     print(str(svc))

# # --------- Control CCCD ------------
# enableNotify = True
# enableIndicate = True
# # ---------------------------------

# try:
#     testService = dev.getServiceByUUID(UUID(0xfff0))
#     for ch in testService.getCharacteristics():
#         print(str(ch))

#     ch = dev.getCharacteristics(uuid=UUID(0xfff4))[0]
#     print("ch:", ch)
#     print("support:", ch.supportsRead())
#     if (ch.supportsRead()):
#         print(ch.read())

#     # --------- add CCCD operation --------
#     print(ch.valHandle)
#     cccd = ch.valHandle + 2

#     if enableNotify:
#         # setup to enable notifications
#         dev.writeCharacteristic(cccd, b"\x01\x00")
#         print("Enable notifications......")
#     if enableIndicate:
#         # setup to enable indications
#         dev.writeCharacteristic(cccd, b"\x02\x00")
#         print("Enable indications......")

#     while True:
#         if dev.waitForNotifications(1.0):
#             # handleNotification() was called
#             continue
#         print("Waiting")

# finally:
#     dev.disconnect()