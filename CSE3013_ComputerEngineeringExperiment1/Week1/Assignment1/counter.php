<?php
	// 카운트 정보가 담긴 파일명을 변수에 할당한다.
	$filename = "counter.txt";

	// 파일에서 이전의 카운트를 읽어온다.
	$fp = fopen($filename, "r");
	$count = fread($fp, filesize($filename));
	fclose($fp);

	// 쿠키값이 기록되어 있지 않으면 쿠키값을 설정하고 카운트를 1만큼 증가시키며 파일에 새로 카운트를 덮어쓴다. 
	if(!$_COOKIE["ip"]){
		setcookie("ip", $_SERVER["REMOTE_ADDR"]);
		$count++;
		$fp = fopen($filename, "w"); // 서버에서 파일을 실행할 때 공개 권한을 쓰기도 가능하게 해야 한다는 점을 유의해야 한다. (-rw-r--rw-)
		fwrite($fp, $count);
		fclose($fp);
	}

	// 카운트를 string 데이터 형식으로 변환하여 변수에 저장한 후 각 자리 수에 해당하는 숫자를 나타내는 jpg 파일을 읽어온다.
	$str = (string)$count;
	echo('<font size="12">방문자 수 : </font>');
	for($i = 0; $i < strlen($str); $i++){
		$num = substr($str, $i, 1);
		echo("<img src='".$num.".JPG'>"); 
	}
?>