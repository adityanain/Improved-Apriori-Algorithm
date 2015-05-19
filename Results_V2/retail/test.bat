FOR %%v IN (1,2,3,4,5) DO ap_sp_final.exe retail.dat %%v >> ap_sp_retail_%%v.txt
FOR %%v IN (1,2,3,4,5) DO ap.exe retail.dat %%v >> ap_retail_%%v.txt

