import test
marian = test.User('Marian', 'hasloMariana')
print marian.join_2_room('pokoj1', 'hasloDoPokoju').__str__()
while True:
    marian.deliver('pokoj1', 'hasloDoPokoju')
    resp = marian.recvDelivery()
    print resp.senderName + str(' : ') + resp.resultDescription
