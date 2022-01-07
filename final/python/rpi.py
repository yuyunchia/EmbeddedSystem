from bluepy.btle import Peripheral, UUID, ADDR_TYPE_PUBLIC, Scanner, DefaultDelegate
import time

DEVICE_MAC = "fd:d2:cc:13:77:3f"
MODE = "random"

def light_mode(mode):
    print(mode)
    if mode == b"r":
        # TODO
        print("Turn right")
        pass
    if mode == b"l":
        # TODO
        print("Turn left")
        pass
    if mode == b"s":
        # TODO
        print("Stopping")
        pass
    if mode == b"n":
        print("Nothing to do")
        pass

def main():
    print("Connecting...")
    dev = Peripheral(DEVICE_MAC, MODE)
    
    print("Services...")
    for svc in dev.services:
        print(str(svc.uuid))

    try:
        while True:
            btnService = dev.getServiceByUUID(0xA000)
            btnCharas = btnService.getCharacteristics()
            print("Num of characteristics: ", len(btnCharas))
            print("Characteristics: ")
            for c in btnCharas:
                print("uuid: %s, content: %s" % (str(c.uuid), str(c.read())))
            for c in btnCharas:
                if not c.supportsRead():
                    print("characteristic with uuid %s is not readible" % (str(c.uuid)))
            for ch in btnService.getCharacteristics():
                print(str(ch))

            Readch = dev.getCharacteristics(uuid=UUID(0xA001))[0]
            print(str(Readch.propertiesToString()))
            cur = ''

            while True:
                if (Readch.supportsRead()):
                    time.sleep(0.1)
                    
                    if( cur != Readch.read()):
                        cur = Readch.read()
                        print("Readch.read(): ", Readch.read())
                        
                        if Readch.read() in [b"r", b"l", b"s", b"n"]:
                            light_mode(Readch.read())


    finally:
        dev.disconnect()

if __name__ == "__main__":
    main()