from bluepy.btle import Peripheral, UUID, ADDR_TYPE_PUBLIC, ADDR_TYPE_RANDOM, Scanner, DefaultDelegate

class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)

    def handleDiscovery(self, dev, isNewDev, isNewData):
        if isNewDev:
            print("Discovered device", dev.addr)
        elif isNewData:
            print("Received new data from", dev.addr)

class MyDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)
        self.handle = handle
    
    def handleNotification(self, cHandle, data):
        if(cHandle == self.handle):
            print("Handle Notification things")



def main():
    print("Connecting... ")
    dev = Peripheral('a8:9c:ed:a0:5e:fb', 'public')

    print("Services...")
    for svc in dev.services:
        print(str(svc.uuid))

    try:
        # while True:
        service = dev.getServiceByUUID(0xfff0)
        charas = service.getCharacteristics()
        print("Num of characteristics: ", len(charas))
        print("Characteristics: ")
        for c in charas:
            print("uuid: %s, content: %s" % (str(c.uuid), str(c.read())))

        for c in charas:
            if not c.supportsRead():
                print("characteristic with uuid %s is not readible" % (str(c.uuid)))
        

        testService = dev.getServiceByUUID(UUID(0xfff0))
        print(testService.getCharacteristics())
        for ch in testService.getCharacteristics():
            print(str(ch))
       

        ch = dev.getCharacteristics(uuid=UUID(0xfff2))[0]
        print(str(ch))
        print(str(ch.propertiesToString()))

     

        dev.setDelegate(MyDelegate)
        dev.writeCharacteristic((ch.getHandle()+1), "\x01\x00".encode())

      
        while True:
            if dev.waitForNotifications(3.0):
                print("success enable")
                continue
              #  break
            else:
                print("fail enable")

        print('dev: ', dev)
        ch = dev.getCharacteristics(uuid=UUID(0xfff2))[0]
        print(ch)
        if (ch.supportsRead()):
            print(ch.read())

    finally:
        dev.disconnect()

if __name__ == "__main__":
    main()
