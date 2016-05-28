import test
olaf = test.User('Olaf', 'hasloOlafa')
print olaf.join_2_room('pokoj', 'hasloDoPokoju').__str__()
while True:
    olaf.deliver('pokoj', 'hasloDoPokoju')
    resp = olaf.recvDelivery()
    print resp.senderName + str(' : ') + resp.resultDescription
