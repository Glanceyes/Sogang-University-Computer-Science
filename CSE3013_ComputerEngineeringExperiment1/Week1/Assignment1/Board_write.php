<?php
	if($_POST) {
		$file = "Data.txt";
		$fp = fopen($file, "w"); // 서버에서 파일 공개 권한을 쓰기가 가능하도록 해야 정상 작동한다.
		fwrite($fp, $_POST["input"]);
		fclose($fp); // Data.txt 파일에 $_POST 전역 변수를 통해 넘어 온 전송 값의 input 데이터를 저장한다.
		Header("Location:./Board_main.php"); // PHP에 내장된 Header함수를 사용하여 원하는 페이지로 이동한다.
	}
	else {
		echo ('<html>
			<head>
				<title>게시판 쓰기</title>
			</head>
			<body>
				게시판 쓰기<br>
				<form id="inputform" method="POST">
					<textarea rows="7" cols="50" name="input"></textarea>
					<br>
					<input type="submit" value="등록하기">
					<input type="reset" value="다시기입">
				</form>
				<br>
				<a href="Board_main.php">되돌아 가기</a>
			</body>
		</html>');
		// POST 방식으로 form 태그에 입력된 데이터를 전송한다. textarea는 7줄과 한 줄 당 50자를 입력할 수 있는 공간을 제공한다. 만일 한 줄 당 50자를 초과하면 다음 줄로 자동으로 내려가고 5줄을 초과하면 자동 스크롤이 생긴다. input 입력란의 데이터 전송 방식에서 '등록하기'는 submit으로, '다시 기입'은 reset으로 설정했다. '되돌아 가기'는 하이퍼링크로 처리했다.
	}
?>
