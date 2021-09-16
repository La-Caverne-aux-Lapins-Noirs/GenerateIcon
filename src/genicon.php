#!/usr/bin/env php
<?php
/*
 ** Genere des images de 400pixels de large et 100 pixel de haut
 ** avec le nom de la fonction réussie a l'interieur.
 **
 ** Le nom de la fonction est haché de manière à choisir un pattern
 ** pour le ruban (a la manière des militaires), et également
 ** de manière à choisir les couleurs
*/
header ("Content-type: image/png");
$width = 400;
$height = 100;

if ($argc == 2)
    $_GET["function"] = $argv[1];

if ($argc == 4)
{
    $width = (int)$argv[2];
    $height = (int)$argv[3];
}

$image = imagecreate($width, $height);


/*
 ** Type de ruban (pour commencer...):
 ** - Uni
 ** - Coupe verticale en 2, 3, 4, 5 ou 6 parties
 ** - Couple horizontale en 2, 3 ou 4 parties
 ** - Damier plus ou moins petit
 ** - Par dessus un des motifs precedent: un ou plus cercles centrés horizontalement
 ** - Par dessus un des motifs precedent: un ou plus cercles concentriques centrés
 ** Facon de colorier:
 ** - Série de couleurs (Si découpe en 4, il faut 4 couleurs)
 ** - Symétrie de couleurs (Si découpe en 5, il en faut 3...)
 ** - Alternance (2 couleurs)
 **

 ** Pour l'instant sont implémenté: la coupe verticale, la coupe horizontale, les series de couleur
*/

$colors = [
    imagecolorallocate($image, 0, 0, 0), // noir
    imagecolorallocate($image, 64, 64, 64), // gris sombre
    imagecolorallocate($image, 128, 128, 128), // gris
    imagecolorallocate($image, 192, 192, 192), // gris clair
    imagecolorallocate($image, 255, 255, 255), // blanc

    imagecolorallocate($image, 64, 0, 0), // rouge très sombre
    imagecolorallocate($image, 128, 0, 0), // rouge sombre
    imagecolorallocate($image, 192, 0, 0), // rouge peu sombre
    imagecolorallocate($image, 255, 0, 0), // rouge
    imagecolorallocate($image, 255, 128, 128), // rouge clair

    imagecolorallocate($image, 0, 64, 0), // vert très sombre
    imagecolorallocate($image, 0, 128, 0), // vert sombre
    imagecolorallocate($image, 0, 192, 0), // vert peu sombre
    imagecolorallocate($image, 0, 255, 0), // vert
    imagecolorallocate($image, 128, 255, 128), // vert clair

    imagecolorallocate($image, 0, 0, 64), // bleu très sombre
    imagecolorallocate($image, 0, 0, 128), // bleu sombre
    imagecolorallocate($image, 0, 0, 192), // bleu peu sombre
    imagecolorallocate($image, 0, 0, 255), // bleu
    imagecolorallocate($image, 128, 128, 255), // bleu clair

    imagecolorallocate($image, 128, 128, 0), // jaune foncé
    imagecolorallocate($image, 255, 255, 0), // jaune
    imagecolorallocate($image, 255, 255, 128), // jaune clair
    imagecolorallocate($image, 0, 128, 128), // cyan foncé
    imagecolorallocate($image, 0, 255, 255), // cyan
    imagecolorallocate($image, 128, 255, 255), // cyan clair
    imagecolorallocate($image, 128, 0, 128), // violet foncé
    imagecolorallocate($image, 255, 0, 255), // violet
    imagecolorallocate($image, 255, 128, 255), // violet clair

    imagecolorallocate($image, 255, 0x64, 0xB4), // rose
];

function drawsquare($image, $x, $y, $w, $h, $c)
{
    imagefilledrectangle($image, $x, $y, $x + $w, $y + $h, $c);
}

function vsplit($img, $hash)
{
    global $colors;
    global $width;
    global $height;

    $split = $hash % 8 + 1;
    $w = $width / $split;
    $h = $height;
    srand($hash);
    for ($i = 0; $i < $split; ++$i)
    {
    	drawsquare($img, $i * $w, 0, $w, $h, $colors[$hash % count($colors)]);
	$hash = crc32($hash);
    }
}

function hsplit($img, $hash)
{
    global $colors;
    global $width;
    global $height;

    $split = $hash % 4 + 1;
    $w = $width;
    $h = $height / $split;
    srand($hash);
    for ($i = 0; $i < $split; ++$i)
    {
	drawsquare($img, 0, $i * $h, $w, $h, $colors[$hash % count($colors)]);
	$hash = crc32($hash);
    }
}

$draw = [
    "vsplit",
    "hsplit",
];

if (!isset($_GET["function"]))
{
    drawsquare($image, 0, 0, 400, 100, imagecolorallocate($image, 0, 0, 255));
    $_GET["function"] = "?";
}
else
{
    $hash = (int)abs(crc32($_GET["function"]));
    $draw[$hash % count($draw)]($image, $hash);
}

function add_text($image, $text = "????")
{
    global $width;
    global $height;

    $font = "/opt/technocore/genicon/unispace.ttf";
    $size = 40;
    $x = ($width - strlen($text) * 30) / 2;
    $y = $size / 2 + $height / 2;
    imagettftext($image, $size, 0, $x - 2, $y - 2, imagecolorallocate($image, 0, 0, 0), $font, $text);
    imagettftext($image, $size, 0, $x + 2, $y - 2, imagecolorallocate($image, 0, 0, 0), $font, $text);
    imagettftext($image, $size, 0, $x - 2, $y + 2, imagecolorallocate($image, 0, 0, 0), $font, $text);
    imagettftext($image, $size, 0, $x + 2, $y + 2, imagecolorallocate($image, 0, 0, 0), $font, $text);
    imagettftext($image, $size, 0, $x, $y, imagecolorallocate($image, 255, 255, 255), $font, $text);
}

add_text($image, $_GET["function"]);
imagepng($image, NULL, 0, PNG_NO_FILTER);
imagedestroy($image);
?>

