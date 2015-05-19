FOR %%v IN (1,2,3,4,5) DO ap_sp_final.exe T10I4D100K.dat %%v >> ap_sp_T10I4D100K_%%v.txt
FOR %%v IN (1,2,3,4,5) DO ap.exe T10I4D100K.dat %%v >> ap_T10I4D100K_%%v.txt

