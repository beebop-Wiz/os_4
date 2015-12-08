def tohex(val, nbits):
  return hex((val + (1 << nbits)) % (1 << nbits))

class MallocListIterator:
    def __init__(self, val):
        self.val = val
        
    def __iter__(self):
        return self

    def __next__(self):
        return ("next", self.val)

class EmptyListIterator:
    def __iter__(self):
        return self

    def __next__(self):
        raise StopIteration
    
class MallocListPrinter:
    def __init__(self, val):
        self.val = val

    def children(self):
        if tohex(-1 & int(self.val['magic'].cast(gdb.lookup_type('long'))), 32) == '0xa10ca7ed':
            return MallocListIterator(gdb.parse_and_eval('(struct malloc_header) *' + str(self.val['next'])))
        else:
            return EmptyListIterator()
        
    def to_string(self):
        st = gdb.find_pc_line(int(self.val['owner'])).symtab
        magic_raw = tohex(-1 & int(self.val['magic'].cast(gdb.lookup_type('long'))), 32)
        type_raw = tohex(-1 & int(self.val['type'].cast(gdb.lookup_type('long'))), 32)
        if magic_raw == '0xa10ca7ed':
            magic = 'ALLOCATED'
        else:
            magic = '<bad>'
        if type_raw == '0x25ed':
            type = 'USED'
        elif type_raw == '0xf4ee':
            type = 'FREE'
        else:
            type = '<bad>'
        if st == None:
            symbol = '<bad>'
        else:
            symbol = str(self.val['owner'])
        return ('magic=' + magic + ' type=' + type + ' owner=' + symbol + ' length=' + str(int(self.val['length'].cast(gdb.lookup_type('long')))))

def lookup_type(val):
    if str(val.type) == 'struct malloc_header':
        return MallocListPrinter(val)
    else:
        return None

gdb.pretty_printers.append(lookup_type)
