from collections import defaultdict
def parse_mdl(mdl_iter):
    ret_dict = defaultdict(list)
    while True:
        try: 
            token = next(mdl_iter)
        except StopIteration:
            return dict(ret_dict)
        if '"' not in token and '{' in token:
            key = token.split('{')[0].strip()
            ret_dict[key].append(parse_mdl(mdl_iter))
        elif '"' not in token and '}' in token:
            return dict(ret_dict)
        else:
            key_val = filter(lambda x: x, token.split('\t'))
            if len(key_val)==1:
                key_val = filter(lambda x: x, token.split())
            key_val = map(lambda x: x.strip(), key_val)
            ret_dict[key_val[0]].append(key_val[1])

with open('test.mdl','r') as mdl_file:
    mdl = mdl_file.read()

#print mdl.split('\n')
#print filter(lambda x: x, mdl.split('\n'))
mdl_list = mdl.split('\n')
mdl_list = filter(lambda x: x, mdl_list)
mdl_iter = iter(mdl_list)
mdl_dict = parse_mdl(mdl_iter)

