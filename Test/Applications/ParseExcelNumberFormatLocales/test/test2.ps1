$numbers = (1025,
1026,
2052,
1028,
1050,
1029,
1030,
1043,
1033,
1061,
1035,
1036,
1031,
1032,
1037,
1081,
1038,
1057,
1040,
1041,
1087,
1042,
1062,
1063,
1086,
1044,
1045,
1046,
2070,
1048,
1049,
2074,
1051,
1060,
3082,
1053,
1054,
1055,
1058,
1066
)
 foreach($i in $numbers) {
 Start-Sleep -Seconds 1
echo Set-Culture $i
 Set-Culture $i

  $pathMacro = $(pwd).Path + "\macros-locale (1).xlsm"
  $pathOutput = $(pwd).Path + "\" + $i + ".txt"
  echo .\run2.vbs $pathMacro $pathOutput
 
  .\run2.vbs $pathMacro $pathOutput
  Start-Sleep -Seconds 1
 
}


Set-Culture ru-Ru
 
 



