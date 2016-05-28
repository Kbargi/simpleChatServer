import test
zbyszek = test.User('Zbyszek', 'hasloZbyszka')
print zbyszek.createRoom('pokoj', 'hasloDoPokoju').__str__()
while True:
    zbyszek.deliver('pokoj', 'hasloDoPokoju')
    resp = zbyszek.recvDelivery()
    print resp.senderName + str(' : ') + resp.resultDescription
