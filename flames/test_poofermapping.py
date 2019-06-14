''' pooferMappings is an object containing poofer names as attributes, where each 
attribute value is a string of 3 digits that translate to the poofer's address on the 
poofer control boards.  The first and second digits is the board number in hexadecimal, 
and the third digit is the channel on that board (there are 8 channels per board).
The required attribute names are:
		NN,NW,NE,NT,EN,EE,ES,ET,SE,SS,SW,ST,WS,WW,WN,WT,TN,TE,TS,TW,TT,BN,BE,BS,BW
and the addresses depend on the number of channels we use on each poofer board. 
'''

mappings = {}
mappings["NT"]="011"
mappings['NW']="012"
mappings['NN']="013"
mappings['NE']="014"
mappings['ST']="015"
mappings['SE']="016"
mappings['SS']="017"
mappings['SW']="018"
mappings['ET']="021"
mappings['EN']="022"
mappings['EE']="023"
mappings['ES']="024"
mappings['WT']="025"
mappings['WS']="026"
mappings['WW']="027"
mappings['WN']="028"
mappings['TT']="031"
mappings['TN']="032"
mappings['TE']="033"
mappings['TW']="034"
mappings['TS']="035"
mappings['BN']="041"
mappings['BE']="042"
mappings['BS']="043"
mappings['BW']="044"

