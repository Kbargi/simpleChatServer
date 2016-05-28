import test
marek = test.User('Marek', 'hasloMarka')
print marek.join_2_room('pokoj', 'hasloDoPokoju').__str__()
while True:
    marek.deliver('pokoj', 'hasloDoPokoju')
    resp = marek.recvDelivery()
    print resp.senderName + str(' : ') + resp.resultDescription
