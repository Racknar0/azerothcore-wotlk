SET @TEXT_ID := 601083;
DELETE FROM `npc_text` WHERE `ID` IN  (@TEXT_ID,@TEXT_ID+1);
INSERT INTO `npc_text` (`ID`, `text0_0`) VALUES
(@TEXT_ID, 'La transfiguración te permite cambiar la apariencia de tus objetos sin cambiar las estadísticas de los mismos.\r\nLos objetos utilizados en la transfiguración ya no son reembolsables, comerciables y están vinculados a ti.\r\nActualizar un menú actualiza la vista y los precios.\r\n\r\nNo todo se puede transfigurar entre sí.\r\nLas restricciones incluyen, pero no se limitan a:\r\nSolo se pueden transfigurar armaduras y armas.\r\nLas armas de fuego, arcos y ballestas se pueden transfigurar entre sí.\r\nLas cañas de pescar no se pueden transfigurar.\r\nDebes ser capaz de equipar ambos objetos utilizados en el proceso.\r\n\r\nLas transfiguraciones permanecen en tus objetos mientras los poseas.\r\nSi intentas poner el objeto en el banco de hermandad o enviarlo por correo a otra persona, la transfiguración se quita.\r\n\r\nTambién puedes eliminar transfiguraciones de forma gratuita en el transfigurador.'),
(@TEXT_ID+1, 'Puedes guardar tus propios conjuntos de transfiguración.\r\n\r\nPara guardarlos, primero debes transfigurar los objetos que tengas equipados.\r\nLuego, cuando vayas al menú de gestión de conjuntos y accedas al menú de guardar conjuntos,\r\ntodos los objetos que hayas transfigurado se mostrarán para que puedas ver lo que estás guardando.\r\nSi consideras que el conjunto está bien, puedes hacer clic para guardarlo y nombrarlo como desees.\r\n\r\nPara usar un conjunto, puedes hacer clic en el conjunto guardado en el menú de gestión de conjuntos y luego seleccionar "usar conjunto".\r\nSi el conjunto tiene una transfiguración para un objeto que ya está transfigurado, se perderá la transfiguración anterior.\r\nTen en cuenta que las mismas restricciones de transfiguración se aplican cuando intentas usar un conjunto como en la transfiguración normal.\r\n\r\nPara eliminar un conjunto, puedes ir al menú del conjunto y seleccionar "eliminar conjunto".');

SET @STRING_ENTRY := 11100;
DELETE FROM `acore_string` WHERE `entry` IN  (@STRING_ENTRY+0,@STRING_ENTRY+1,@STRING_ENTRY+2,@STRING_ENTRY+3,@STRING_ENTRY+4,@STRING_ENTRY+5,@STRING_ENTRY+6,@STRING_ENTRY+7,@STRING_ENTRY+8,@STRING_ENTRY+9,@STRING_ENTRY+10, @STRING_ENTRY+11, @STRING_ENTRY+12, @STRING_ENTRY+13, @STRING_ENTRY+14, @STRING_ENTRY+15, @STRING_ENTRY+16);
INSERT INTO `acore_string` (`entry`, `content_default`) VALUES
(@STRING_ENTRY+0, '¡Woohoo! ¡Objeto transfigurado satisfactoriamente!'),
(@STRING_ENTRY+1, 'Ups... ¡La ranura de equipo está más vacía que mi billetera en lunes!'),
(@STRING_ENTRY+2, '¡Hey! ¿Qué intentas hacer con ese objeto de origen inválido? ¡Eso no se vale!'),
(@STRING_ENTRY+3, '¡Oh no! ¡El objeto de origen ha desaparecido como un mago en un truco!'),
(@STRING_ENTRY+4, '¡Ups! ¡El objeto de destino ha salido a tomar un café y no está disponible en este momento!'),
(@STRING_ENTRY+5, '¡Ay, caramba! ¡Los objetos seleccionados son tan inválidos como mi intento de hacer ejercicio!'),
(@STRING_ENTRY+6, '¡Lo siento, amigo! ¡Tu cuenta bancaria parece más vacía que mi estómago después de una fiesta!'),
(@STRING_ENTRY+7, '¡Ups! ¡Parece que tus fichas se han escapado como un gato travieso!'),
(@STRING_ENTRY+8, '¡Ups! ¡Todas tus transfiguraciones han desaparecido como un mal sueño!'),
(@STRING_ENTRY+9, '¡Hey! ¡¿Dónde está mi transfiguración?! ¡No se encontró nada por aquí!'),
(@STRING_ENTRY+10, '¡Oh no! ¡Parece que has escrito un nombre más raro que el mío!'),
(@STRING_ENTRY+11, '¡Ey! ¡Mira estas transfiguraciones tan chulas! ¡Reconecta para verlas en acción!'),
(@STRING_ENTRY+12, '¡Chispas! ¡Las transfiguraciones han desaparecido! ¡Reconecta para recuperar la vista!'),
(@STRING_ENTRY+13, '¡Hey, amigo! ¡Ese objeto no es adecuado para la transfiguración! ¡Pero sigue intentando!'),
(@STRING_ENTRY+14, '¡Alto ahí! ¡Ese objeto no se puede usar para transfiguración del jugador objetivo! ¡Mira a otro lado!'),
(@STRING_ENTRY+15, '¡Redoble de tambores, por favor! ¡Vamos a sincronizar las apariencias transfiguradas!'),
(@STRING_ENTRY+16, '¡Fiesta de confeti! ¡Sincronización de apariencia completada con éxito! ¡Yuhuu!');

DELETE FROM `command` WHERE `name` IN ('transmog', 'transmog add', 'transmog sync', 'transmog add set');
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('transmog', 0, 'Syntax: .transmog <on/off>\nAllows seeing transmogrified items and the transmogrifier NPC.'),
('transmog add', 1, 'Syntax: .transmog add $player $item\nAdds an item to a player\'s appearance collection.'),
('transmog sync', 0, 'Syntax: .transmog sync\nSyncs transmog addon appearances with the server.'),
('transmog add set', 1, 'Syntax: .transmog add set $player $itemSet\nAdds items of an item set to a player\'s appearance collection.');
