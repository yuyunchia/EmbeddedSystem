from bluepy.btle import Peripheral, UUID, ADDR_TYPE_PUBLIC, ADDR_TYPE_RANDOM
from bluepy.btle import Scanner, DefaultDelegate

class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)
    def handleDiscovery(self, dev, isNewDev, isNewData):
        if isNewDev:
            print ("Discovered device", dev.addr)
        elif isNewData:
            print ("Received new data from", dev.addr)
                
scanner = Scanner().withDelegate(ScanDelegate())
scanner.clear()

devices = scanner.scan(10.0)
n=0
for dev in devices:
    print ("%d: Device %s (%s), RSSI=%d dB") % (n, dev.addr, dev.addrType, dev.rssi)
    n += 1
    for (adtype, desc, value) in dev.getScanData():
        print ("  %s = %s") % (desc, value)
number = input('Enter your device number: ')

mappedAddrType = ''

print('Device', number)
print(devices[number].addr)

# if (addr_type == 'public') :
#     mappedAddrType = ADDR_TYPE_PUBLIC

# if (addr_type == 'random') :
#     mappedAddrType = ADDR_TYPE_RANDOM


print ("Connecting...")
dev = Peripheral(devices[number].addr, 'public')

print ("Services...")
for svc in dev.services:
    print (str(svc))

try:
    testService= dev.getServiceByUUID(UUID(0xfff0))
    print (testService.getCharacteristics())
    for ch in testService.getCharacteristics():
        print (str(ch))
    
        
    ch= dev.getCharacteristics(uuid=UUID(0xfff1))[0]
    if (ch.supportsRead()):
        print (ch.read())
        
finally:
   dev.disconnect()
