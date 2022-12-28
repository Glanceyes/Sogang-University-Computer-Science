<?php
	$file = "Data.txt";
	$fp = fopen($file, "r");
	$txt = fread($fp, filesize($file));
	fclose($fp);
	// Data.txt 파일에서 데이터를 읽어 오고 변수에 할당한다.
	echo('<html>
			<head>
				<title>게시판 읽기</title>
			</head>
			<body>
				게시판 읽기<br>
				<textarea rows="7" cols="50">'.$txt.'</textarea>
				<br>
				<a href="Board_main.php">되돌아 가기</a>
			</body>
		</html>');
	// textarea 영역에 파일에 저장했던 데이터를 기본 값으로 불러온다. 그리고 '되돌아 가기'를 누르면 Board_main.php로 이동한다.
?>
