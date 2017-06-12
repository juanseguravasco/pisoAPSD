<?php
	ini_set("display_errors", 0);

	require_once "connexio.inc";	
	$mysqli = new mysqli($servidor, $usuario, $contrasenya, $basedades) or die("Error de connexio amb la base de dades ".$mysqli->connect_error);
	$mysqli->set_charset('utf8');

	$consulta = "select CONCAT(apellidos, ', ', nombre) As usuario from usuarios where ".$_REQUEST['campo']."='".$_REQUEST['codigo']."' AND activo=1";

	$rs = $mysqli->query($consulta) or die ("No s'ha pogut executar la consulta: '".$consulta."'");
	if ($rs->num_rows==0) {
		echo "0";
	} else {
		$registre = $rs->fetch_assoc();
		echo $registre['usuario'];
	}
	$rs->free();
	$mysqli->close();
?>
