<?php
	$data = '<html>
		<head>
			<title>미니 보드</title>
		</head>
		<body>
			<a href="Board_write.php">게시판 쓰기</a><br>
			<a href="Board_read.php">게시판 읽기 </a>
		</body>
	</html>';
	echo($data);
	// '게시판 쓰기'를 클릭하면 Board_write.php로 이동하고, '게시판 읽기'를 클릭하면 Board_read.php로 이동한다.
?>