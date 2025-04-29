import os
import sys

if __name__ == '__main__':

	aliases = [ [ [   0,    9], 'str'],		[ [  10,   19], 'dbl'],		[ [  20,   29], 'dbl'],		[ [  30,   37], 'dbl'],
				[ [  38,   59], 'dbl'],		[ [  60,   79], 'i16'],		[ [  90,   99], 'i32'],		[ [ 100,  102], 'str'],
				[ [ 105,  105], 'hex_str'],	[ [ 110,  119], 'dbl'],		[ [ 120,  129], 'dbl'],		[ [ 130,  139], 'dbl'],
				[ [ 140,  149], 'dbl'],		[ [ 160,  169], 'i64'],		[ [ 170,  179], 'i16'],		[ [ 210,  239], 'dbl'],
				[ [ 270,  279], 'i16'],		[ [ 280,  289], 'i16'],		[ [ 290,  299], 'boolean'],	[ [ 300,  309], 'str'],
				[ [ 310,  319], 'hex_str'],	[ [ 320,  329], 'hex_str'],	[ [ 330,  369], 'hex_str'],	[ [ 370,  379], 'i16'],
				[ [ 380,  389], 'i16'],		[ [ 390,  399], 'hex_str'],	[ [ 400,  409], 'i16'],		[ [ 410,  419], 'str'],
				[ [ 420,  429], 'i32'],		[ [ 430,  439], 'str'],		[ [ 440,  449], 'i32'],		[ [ 450,  459], 'i32'],
				[ [ 460,  469], 'dbl'],		[ [ 470,  479], 'str'],		[ [ 480,  481], 'hex_str'],	[ [ 999,  999], 'str'],
				[ [1000, 1003], 'str'],		[ [1004, 1004], 'hex_str'],	[ [1005, 1005], 'str'],		[ [1010, 1013], 'dbl'],
				[ [1020, 1023], 'dbl'],		[ [1030, 1033], 'dbl'],		[ [1040, 1042], 'dbl'],		[ [1070, 1070], 'i16'],
				[ [1071, 1071], 'i32'] ]

	f = open('type_alias.txt', 'wb')
	f.write('\t// Auto generated file\n'.encode())
	f.write('\t// Do not modify\n\n'.encode())
	for rng in aliases:
		for i in range(rng[0][0], rng[0][1]+1):
			type = {
				'dbl': 'double',
				'i16': 'std::int16_t',
				'i32': 'std::int32_t',
				'i64': 'std::int64_t',
				'boolean': 'bool',
				'str': 'string_t',
				'hex_str': 'binary_t'
			}.get(rng[1], 'unknown')
			f.write(f'\tusing gcv_{i:03} = group_code_value_t<{i:3}>; // {type}\n'.encode())

