from bluepy.btle import Peripheral, DefaultDelegate

class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)

    def handleDiscovery(self, cHandle, data):
        print(f"Data from handle {cHandle}: {data}")

def main():
    print("Connecting...")
    dev = Peripheral('a8:9c:ed:a0:5e:fb', 'public')
    dev.setDelegate(ScanDelegate())

    print("Services...")
    for svc in dev.services:
        print(str(svc.uuid))

    try:
        # while True:
        descriptor = dev.getDescriptors()
        print("Num of discriptors: %d" % (len(descriptor)))
        print("Descriptors: ")
        for d in descriptor:
            print("handle: %s, uuid: %s" % (str(d.handle), str(d.uuid)))
            if "2902" in str(d.uuid):
                print("Writing to uuid: %s" % (str(d.uuid)))
                # print(dev.writeCharacteristic(d.handle, b"\x01\x00", withResponse=True))
                print(d.write(b"\x01\x00", withResponse=True))

        while True:
            if dev.waitForNotifications(1.0):
                continue
            print("Waiting...")

        # service = dev.getServiceByUUID(0xfff0)
        # charas = service.getCharacteristics()
        # print("Num of characteristics: ", len(charas))
        # print("Characteristics: ")
        # for c in charas:
        #     print("uuid: %s, property: %s" % (str(c.uuid), c.propertiesToString()))
        #     print(c.read())
        # for c in charas:
        #     if "NOTIFY" in c.propertiesToString():
        #         print("characteristic with uuid %s is not readible" % (str(c.uuid)))
                # print(dev.handleNotification(c.getHandle(), c.read()))

    finally:
        dev.disconnect()

if __name__ == "__main__":
    main()
