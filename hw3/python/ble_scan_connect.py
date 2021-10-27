from bluepy.btle import Peripheral, UUID, ADDR_TYPE_PUBLIC, ADDR_TYPE_RANDOM, Scanner, DefaultDelegate

class MyDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)

    def handleNotification(self, cHandle, data):
        print(data)

per = Peripheral("ea:c7:bd:19:0a:83","random")
per.setDelegate(MyDelegate())

print("Connected")

try:
    per.writeCharacteristic(14,bytes('aa','utf-8'))
    print("writing done")
    while True:
        if per.waitForNotifications(1.0):
            print("Notification")
            continue
    print("Waiting")
finally:
    per.disconnect()
    print ("Disconnected")

# class ScanDelegate(DefaultDelegate):
#     def __init__(self):
#         DefaultDelegate.__init__(self)

#     def handleDiscovery(self, dev, isNewDev, isNewData):
#         if isNewDev:
#             print("Discovered device", dev.addr)
#         elif isNewData:
#             print("Received new data from", dev.addr)

# def main():
#     print("Connecting...")
#     dev = Peripheral('a8:9c:ed:a0:5e:fb', 'public')
#     delegate = DefaultDelegate()

#     print("Services...")
#     for svc in dev.services:
#         print(str(svc.uuid))

#     try:
#         # while True:
#         descriptor = dev.getDescriptors()
#         print("Num of discriptors: %d" % (len(descriptor)))
#         print("Descriptors: ")
#         for d in descriptor:
#             print("handle: %s, uuid: %s" % (str(d.handle), str(d.uuid)))
#             if "2902" in str(d.uuid):
#                 print("Writing to uuid: %s" % (str(d.uuid)))
#                 dev.writeCharacteristic(d.handle, "\x01\x00".encode())

#         print(dev.__dict__)
#         service = dev.getServiceByUUID(0xfff0)
#         charas = service.getCharacteristics()
#         print("Num of characteristics: ", len(charas))
#         print("Characteristics: ")
#         for c in charas:
#             print("uuid: %s, property: %s" % (str(c.uuid), c.propertiesToString()))
#             print(c.read())
#         # for c in charas:
#         #     if "NOTIFY" in c.propertiesToString():
#         #         print("characteristic with uuid %s is not readible" % (str(c.uuid)))
#         #         print(delegate.handleNotification(c.getHandle(), c.read()))
        

#         # testService = dev.getServiceByUUID(UUID(0xfff0))
#         # print(testService.getCharacteristics())
#         # for ch in testService.getCharacteristics():
#         #     print(str(ch))
#         # # for ds in testService.getDescriptors():
#         # #     print (str(ds))


#         # ch = dev.getCharacteristics(uuid=UUID(0xfff2))[0]
#         # print(str(ch))
#         # print(str(ch.propertiesToString()))
#         # dev.writeCharacteristic(ch.getHandle()+1, "\x01\x00".encode())
#         # print('dev: ', dev)
#         # ch = dev.getCharacteristics(uuid=UUID(0xfff2))[0]
#         # print(ch)
#         # if (ch.supportsRead()):
#         #     print('puppy', ch.read())

#     finally:
#         dev.disconnect()

# if __name__ == "__main__":
#     main()
