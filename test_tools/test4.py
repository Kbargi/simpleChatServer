import test
wieslaw = test.User('Wieslaw', 'hasloWieslawa')
print wieslaw.createRoom('pokoj1', 'hasloDoPokoju').__str__()
while True:
    wieslaw.deliver('pokoj1', 'hasloDoPokoju')
    resp = wieslaw.recvDelivery()
    print resp.senderName + str(' : ') + resp.resultDescription
