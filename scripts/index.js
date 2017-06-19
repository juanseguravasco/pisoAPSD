'use strict'

var servidor='app/fopen.php';
//var servidor='app/datosPrueba.php';

var intervalo=3000;
var tiempos=[];		// Para temporizadores de caudal, ...

$(function(){
	$( "#tabs" ).tabs();
	$( ".widget button" )
	      .eq( 0 ).button( {
	        icon: "ui-icon-arrowthickstop-1-n",
	        showLabel: false
	      } )
	      .end().eq( 1 ).button( {
	        icon: "ui-icon-arrowthick-1-n",
	        showLabel: false
	      } )
	      .end().eq( 2 ).button( {
	        icon: "ui-icon-arrowthick-1-s",
	        showLabel: false
	      } )
	      .end().eq( 3 ).button( {
	        icon: "ui-icon-arrowthickstop-1-s",
	        showLabel: false
	      } )

	pideDatos();
	setInterval(pideDatos, intervalo);
	
	$('.btn-auto').on('change', toogleAuto);
	$('.btn-on').on('change', toogleOn);
	$(".widget button").on('click', function() {
		var valor=$(this).val();
		$.ajax({
			url: servidor,
			data: {arduino: 8, sensor: 9, valor: valor},
			method: 'POST'
		}).then(function(res) {
			procesaRespuesta(res);
		}, function(res) {
			console.log(res);
		})
	})
	$('#btn-reset-alarma').hide();
	$('#btn-reset-alarma').on('click', function() {
		$('#img-alarma').attr('src', 'images/luz-off.png');
		$('#txt-alarma').empty();	
		$('#btn-reset-alarma').hide();
	});
	$('#btn-ent-abrir').on('click', function() {
		abrePuerta("C");
	})
});

function toogleAuto(event) {
	if ($(this).prop('checked')) {
		$(this).parent().next().find('input').attr('disabled', 'disabled');
	} else {
		$(this).parent().next().find('input').removeAttr('disabled');
	}
}

function toogleOn(event) {
	var arduino=getSensores($(this).parents('fieldset').attr('id'));
	var sensor=getSensor('nombre', event.target.id, arduino.sensores).codigo;
	var valor=($(this).prop('checked')?1:0);
	var imagen=$(this).parent().next();

	accionaActuador(arduino.codigo, sensor, valor, imagen);
}

function camelCased(cadena) {
	return cadena.replace(/(-|\ )(([a-z]|[0-9]))/g, function (g) { return g[1].toUpperCase(); });
}

function pideDatos() {
	var arduinos=[2,3,4,5,6,7,8,"A","B","C"];
	var arduinos=[2, 3, 8, "C"];
//	var arduinos=[2];

	console.log("Pidiendo...");
	var mal=0;
	for (var i in arduinos) {
		$.ajax({
//			async: false,
			url: servidor,
			data: {arduino: arduinos[i], sensor: 0, valor: 0},
			method: 'POST'
		}).then(function(res) {
			procesaRespuesta(res);
		}, function(res) {
			console.log("Respuesta errónea de un arduino: "+res);
		})

	console.log("...Pedido "+mal);
	}
}

function muestraSensoresNormal(cadenaDatos) {
	var i, datosSensor, sensor, valor, nombreSensor, datosArduino;
	// limpiamos un poco la cadena. A veces pone -- en vez de -
	cadenaDatos=cadenaDatos.replace('--','-').replace(' ','');
	var respuesta=cadenaDatos.split(":");
	if (respuesta[0]=="8") {
		// Es el arduino de la persiana y pintamos su valor
		pintaPersiana(respuesta[1].split("-")[1]);
		return;
	}
	datosArduino=getSensores(respuesta[0]);
	for (i=1; i<respuesta.length; i++) {
		datosSensor=respuesta[i].split("-")[0];
		valor=respuesta[i].split("-")[1];
		if (valor.toUpperCase()=="X")
			valor=0;
		else if (!isNaN(valor))
			valor=Number(valor);
		if (datosSensor=="" || valor==="")
			continue;
		sensor=getSensor('codigo', datosSensor, datosArduino.sensores);
		if (!sensor.codigo) {
			console.log("Sensor desconocido "+datosSensor+' ('+cadenaDatos+')');
			continue;
		}
		nombreSensor=sensor.nombre;
		// Actualizamos la página con el valor
		switch (sensor.alarma) {
			case 'movimiento':
				if (valor==0) {
					$('#'+nombreSensor).removeClass("verde");				
					$('#'+nombreSensor).css('background-color', '#33b5e5');
				} else {
					$('#'+nombreSensor).addClass("verde");
					$('#'+nombreSensor).css('background-color', 'lime');
				}
				break;
			case 'activo':
				switch (valor) {
					case 0:
						$('#'+nombreSensor).removeClass("rojo");
						break;
					case 1:
						$('#'+nombreSensor).addClass("rojo");
						activaAlarma(sensor, valor);
						break;
				}
				break;
			case 'valor':
				$('#'+nombreSensor).val(valor);
				if (sensor.limite && valor>sensor.limite) {
					activaAlarma(sensor, valor);
				}
				break;
			case 'tiempo':
				$('#'+nombreSensor).val(valor);
				if (valor=="0") {
					tiempos[nombreSensor]=Math.floor(Date.now()/1000);					
				} else {
					if (tiempos[nombreSensor]) {
						var segundos=Math.floor(Date.now()/1000)-tiempos[nombreSensor];
						if (sensor.limite && segundos>sensor.limite) {
							activaAlarma(sensor, segundos);
						}
					} else {
						tiempos[nombreSensor]=Math.floor(Date.now()/1000);
					}					
				}
				break;
			case 'persiana':
				pintaPersiana(valor);
				break;
			default:
				$('#'+nombreSensor).val(valor);
				break;
		}
		// Actualizamos la imagen
		if (sensor.nombre=='coci-freg-caudal') {
			var imagen=$('#'+nombreSensor).parent().parent().nextAll().filter('img');
			if (valor) {
				imagen.attr('src',imagen.attr('src').replace('-off.','-on.'));				
			} else {
				imagen.attr('src',imagen.attr('src').replace('-on.','-off.'));				
			}
		}
	}
}

function controlaPuerta(cadenaDatos) {
	var valores=[];
	var abrir=0;
	var nombreOpcion="";
	var respuesta=cadenaDatos.trim().split(":");
	// Si es la respuesta de puerta abierta la pintamos
	if (respuesta[1]=="On") {
		// Pintamos la puerta abierta y llamamos a una  función en 4 seg. que pinte la puerta cerrada
		$('#btn-ent-abrir').attr('disabled', 'disabled');
		$('#img-puerta').attr('src','images/exit.png');
		$('#img-puerta').parent().removeClass("rojo").addClass("verde");
		setTimeout(function() {
			$('#btn-ent-abrir').removeAttr('disabled');
			$('#img-puerta').attr('src','images/enter.png');
			$('#img-puerta').parent().removeClass("verde").addClass("rojo");
		}, 2000);
	} else if (respuesta[1]=="Off") {
		$('#btn-ent-abrir').removeAttr('disabled');
		$('#img-puerta').attr('src','images/enter.png');
		$('#img-puerta').parent().removeClass("verde").addClass("rojo");
	} else {
		// Si es el dato leído lo procesamos
		var valor=respuesta[1].split("-")[1];
		if (!valor || valor=="n" || valor=="0") {
			// No hay intento de abrir
			return;
		}
		switch (respuesta[0]) {
			case "A":
				nombreOpcion="huella";
				break;
			case "B":
				nombreOpcion="teclado";
				break;
			case "C":
				nombreOpcion="RFID";
				break;
		}
		$('#entrada-'+nombreOpcion).prop('checked','checked');
		$('#valor-'+nombreOpcion).text("("+valor+")"); //respuesta[1]);

		$.ajax({
			url: 'app/compruebaCodigo.php',
			data: {campo: nombreOpcion.toLowerCase(), codigo: valor}, //respuesta[1]},
			method: 'POST'
		}).then(function(res) {
			if (res==0) {
				$('#valor-'+nombreOpcion).removeClass("verde").addClass("rojo");
			} else {
				$('#valor-'+nombreOpcion).text(res).removeClass("rojo").addClass("verde");
				abrePuerta(respuesta[0]);
			}
		}, function(res) {
			console.log("error servidor BBDD: "+res);
		});

	}
}

function getSensor(campo, valor, sensores) {
	var dato={};
	$(sensores).each(function(index, elemento) {
		if (elemento[campo]==valor) {
			dato=elemento;
			return false;
		}
	})
	return dato;
}

function getSensores(buscado) {
	var campoBusqueda;
	var datos={codigo: 0};
	if (buscado.length==1) {
		campoBusqueda="arduino";
	} else {
		campoBusqueda="descrip";		
	}
	$(valoresArduinos).each(function(index, elemento) {
		if (elemento[campoBusqueda]==buscado) {
			datos={
				codigo: elemento.arduino,
				nombre: elemento.descrip,
				sensores: elemento.elementos
			};
			return false;
		}
	})
	return datos;
}

function activaAlarma(sensor, valor) {
	if ($('#ent-auto-alarma')) {	// Si está en manual no se hace nada
		var tiempo = new Date();
		var ahora = tiempo.getHours()+':'+tiempo.getMinutes()+':'+tiempo.getSeconds();
		$('#img-alarma').attr('src', 'images/luz-roja.png');
//		$('#txt-alarma').prepend(ahora+' - '+sensor.descrip+': '+valor+(sensor.limite?' (limite '+sensor.limite+')':'')+'<br>');
		$('#btn-reset-alarma').show();
	}
}

function accionaActuador(arduino, sensor, valor, imagen) {
	$.ajax({
		url: servidor,
		data: {arduino: arduino, sensor: sensor, valor: valor},
		method: 'POST'
	}).then(function(res) {
		switch (res.trim()) {
			case "On":
				imagen.attr('src',imagen.attr('src').replace('-off.','-on.'));
				break;
			case "Off":
				imagen.attr('src',imagen.attr('src').replace('-on.','-off.'));
				break;
			default:
				alert(res);
		}
		console.log(res);
	}, function(res) {
		console.log(res);
	})
}

function pintaPersiana(valor) {
	if (!valor || isNaN(valor) || valor.length>1)
		return;
	$('#persiana').attr('src','images/window-'+valor+'.png');
	$( ".widget button" ).removeAttr('disabled');
	if (valor=='1') {
		$( ".widget button" ).eq(0).attr('disabled','disabled');
	}
	if (valor=='5') {
		$( ".widget button" ).eq(4).attr('disabled','disabled');
	}	
}

function abrePuerta(arduino) {
	$.ajax({
		url: servidor,
		data: {arduino: arduino, sensor: 9, valor: 1},
		method: 'POST'
	}).then(function(res) {
		consle.log(res);
		procesaRespuesta(res);
	}, function(res) {
		alert(res);
	});
	// Como ahora no llega la respuesta marcamos a pelo que se abre
		// Pintamos la puerta abierta y llamamos a una  función en 4 seg. que pinte la puerta cerrada
		$('#btn-ent-abrir').attr('disabled', 'disabled');
		$('#img-puerta').attr('src','images/exit.png');
		$('#img-puerta').parent().removeClass("rojo").addClass("verde");
		setTimeout(function() {
			$('#btn-ent-abrir').removeAttr('disabled');
			$('#img-puerta').attr('src','images/enter.png');
			$('#img-puerta').parent().removeClass("verde").addClass("rojo");
		}, 2000);
}

function procesaRespuesta(res) {
	res=res.trim();
	if (res.charAt(res.length-1)==";")
		res=res.substr(0,res.length-1);		// Quitamos el ; final
	// separamos la cadena por ; por si se ha leído más de 1 arduino
	res.split(";").forEach(function(dato) {
		dato=dato.trim();
		if (dato.charAt(1)!=":") {
			return;
		}
var tiempo = new Date();
var ahora = tiempo.getHours()+':'+tiempo.getMinutes()+':'+tiempo.getSeconds();
$('#txt-alarma').prepend(ahora+' - '+res+'<br>');
		var arduino=dato.split(":")[0];
		if (arduino.length != 1) {
			console.log("Respuesta errónea del arduino "+arduino+": "+res);
		} else {
			switch (arduino) {
				case "2":
				case "3":
				case "4":
				case "5":
				case "6":
				case "7":
				case "8":
					muestraSensoresNormal(dato);
					break;
				case "A":
				case "B":
				case "C":
					controlaPuerta(dato);
					break;
				default:
					console.log("Respuesta errónea de un arduino: "+res);
			}

		}
	})
}