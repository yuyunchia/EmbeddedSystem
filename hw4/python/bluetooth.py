from bluepy.btle import Peripheral, UUID, ADDR_TYPE_PUBLIC, ADDR_TYPE_RANDOM, Scanner, DefaultDelegate
import time 

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

    dev = Peripheral('fd:d2:cc:13:77:3f', 'random')

    print("Services...")
    for svc in dev.services:
        print(str(svc.uuid))

    try:
        while True:
            Btnservice = dev.getServiceByUUID(0xA000)
            Btncharas = Btnservice.getCharacteristics()
            print("Num of characteristics: ", len(Btncharas))
            print("Characteristics: ")
            for c in Btncharas:
                print("uuid: %s, content: %s" % (str(c.uuid), str(c.read())))

            for c in Btncharas:
                if not c.supportsRead():
                    print("characteristic with uuid %s is not readible" % (str(c.uuid)))
            
            for ch in Btnservice.getCharacteristics():
                print(str(ch))

            
            Readch = dev.getCharacteristics(uuid=UUID(0xA001))[0]
            print(str(Readch.propertiesToString()))
            
            Ledch  = dev.getCharacteristics(uuid=UUID(0xA002))[0]
            
            cur = ''
            
            index = 0
            while True:
                if (Readch.supportsRead()):
                        
                    time.sleep(0.1)
                    
                    if( cur != Readch.read()):
                        cur = Readch.read()
                        print("Readch.read(): "Readch.read())
                        
                        if (Readch.read() == b' '):
                            print("get empty")
                            Ledch.write(b"\x01")
                            time.sleep(5)
                            Ledch.write(b"\x00")
                            
                        
                        if (Readch.read() == b'\n'):
                            print("end")
                            return
            

    finally:
        dev.disconnect()

if __name__ == "__main__":
    main()
