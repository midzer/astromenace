/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2018 Mikhail Kurinnoi, Viewizard


	AstroMenace is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	AstroMenace is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/


#include "game.h"




//------------------------------------------------------------------------------------
// локальные переменные
//------------------------------------------------------------------------------------

// прорисовка хинтов во время загрузки
bool NeedShowHint = false;
static const char *LoadingHints[] =
{"9_16Line1",
"9_16Line2",
"9_16Line3",
"9_16Line4",
"9_16Line5",
"9_16Line6"};
#define LoadingHintsCount (int)(sizeof(LoadingHints)/sizeof(LoadingHints[0]))




//------------------------------------------------------------------------------------
// локальная структура данных загрузки
//------------------------------------------------------------------------------------
struct LoadList
{
	// имя файла
	char	FileName[MAX_PATH];
	// тип файла (0-2д текстура, 1-текстура, 2-VW3D, 3-music, 4-sfx)
	int		FileType;
	// вес данного объекта при загрузки (для текстур берем кбайты чистые, т.е. если она в рав формате типа бмп)
	int		Value;
	// альфа канал, если нужно
	bool	Alpha;
	// цвет альфа канала
	BYTE Red, Green, Blue;
	// режим создания альфа канала
	int		AlphaMode;
	// фильтр текстуры
	int		TextWrap;
	int		TextFiltr;
	bool	MipMap;
	// нужно ли для этой текстуры сжатие
	bool	NeedCompression;
	// для 3д моделей, если 1.0f делать структуру с мелкими треугольниками
	float	TriangleSizeLimit;
	// для 3д моделей, если надо - делаем тангенты и бинормали
	bool	NeedTangentAndBinormal;
};
// ВАЖНО!!!
// 1) текстуры должны стоять первые, а модели последние
// 2) одна и таже модель не должна вызываться с разными начальными углами и ресайзом. Для таких случаев нужно 2 модели (одинаковые)... иначе будут проблемы с VBO




// подключаем список загрузки
LoadList	*CurrentList = 0;
unsigned int CurrentListCount = 0;









//------------------------------------------------------------------------------------
// данные загрузки шейдеров
//------------------------------------------------------------------------------------
eGLSL 	*GLSLShaderType1 = 0;
eGLSL 	*GLSLShaderType2 = 0;
eGLSL 	*GLSLShaderType3 = 0;
int 	UniformLocations[100];

struct sGLSLLoadList
{
	char Name[MAX_PATH];
	char VertexShaderFileName[MAX_PATH];
	char FragmentShaderFileName[MAX_PATH];
};
static sGLSLLoadList GLSLLoadList[] =
{
{"ParticleSystem", "glsl/particle.vert", "glsl/particle.frag"},
{"SpaceStars", "glsl/particle_stars.vert", "glsl/particle.frag"},
{"PerPixelLight", "glsl/light.vert", "glsl/light.frag"},
{"PerPixelLight_ShadowMap", "glsl/light_shadowmap.vert", "glsl/light_shadowmap.frag"},
{"PerPixelLight_Explosion", "glsl/light_explosion.vert", "glsl/light_explosion.frag"},
};
#define GLSLLoadListCount sizeof(GLSLLoadList)/sizeof(GLSLLoadList[0])







//------------------------------------------------------------------------------------
// данные загрузки меню
//------------------------------------------------------------------------------------


#define TEXTURE_NO_MIPMAP	RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_NONE

// лист загрузки меню
static LoadList MenuLoadList[] =
{
// текстуры меню... кнопки, диалоги, название игры
{"menu/astromenace.tga",			0, 512, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button384_back.tga",		0, 192, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button384_in.tga",			0, 96, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button384_out.tga",			0, 96, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button256_back.tga",		0, 192, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button256_in.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button256_out.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button256_off.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/blackpoint.tga",			0, 0, true,  255,255,255, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/whitepoint.tga",			0, 0, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/line.tga",					0, 4, true,  0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/perc.tga",					0, 2, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/perc_none.tga",				0, 2, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/checkbox_main.tga",			0, 5, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/checkbox_in.tga",			0, 5, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
// иконки для отображения в меню credits
{"credits/sdl.tga",				0, 32, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"credits/git.tga",				0, 16, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"credits/opengl.tga",				0, 32, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"credits/openal.tga",				0, 32, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"credits/codeblocks.tga",			0, 16, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"credits/gnugcc.tga",				0, 16, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"credits/gentoo.tga",				0, 16, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"credits/freetype.tga",			0, 16, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"credits/oggvorbis.tga",			0, 32, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"credits/gimp.tga",				0, 32, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
// панельки и кнопки вывода информации в меню модернизации корабля
{"menu/workshop_panel1.tga",		0, 270, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/workshop_panel1+.tga",		0, 270, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/workshop_panel2.tga",		0, 492, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/workshop_panel2+.tga",		0, 492, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/workshop_panel3.tga",		0, 110, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/workshop_panel4.tga",		0, 54, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/workshop_panel5.tga",		0, 899, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/ammo.tga",					0, 8, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"lang/en/menu/button_weaponry_out.tga",	0, 6, false, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"lang/en/menu/button_weaponry_in.tga",	0, 6, false, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"lang/de/menu/button_weaponry_out.tga",	0, 6, false, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"lang/de/menu/button_weaponry_in.tga",	0, 6, false, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"lang/ru/menu/button_weaponry_out.tga",	0, 6, false, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"lang/ru/menu/button_weaponry_in.tga",	0, 6, false, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"lang/pl/menu/button_weaponry_out.tga",	0, 6, false, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"lang/pl/menu/button_weaponry_in.tga",	0, 6, false, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/back_spot.tga",				0, 256, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/back_spot2.tga",			0, 256, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
// диалоги
{"menu/dialog512_256.tga",			0, 706, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/dialog512_512.tga",			0, 1242, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/dialog768_600.tga",			0, 2131, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
// панельки
{"menu/panel444_333_back.tga",		0, 639, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/panel444_333_border.tga",	0, 705, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/panel800_444_back.tga",		0, 1631, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
// малые кнопки для диалогов и панелек
{"menu/button_dialog200_out.tga",	0, 57, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button_dialog200_in.tga",	0, 57, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button_dialog200_off.tga",	0, 57, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button_dialog128_out.tga",	0, 39, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button_dialog128_in.tga",	0, 39, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button_dialog128_off.tga",	0, 39, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/arrows_blue.tga",			0, 69, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/arrow_list_up.tga",		0, 69, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/arrow_list_down.tga",		0, 69, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
// иконки отображения в меню выбора миссий
{"script/mission1_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"script/mission2_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"script/mission3_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"script/mission4_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"script/mission5_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"script/mission6_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"script/mission7_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"script/mission8_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"script/mission9_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"script/mission10_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"script/mission11_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"script/mission12_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"script/mission13_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"script/mission14_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"script/mission15_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
// иконки систем корабля
{"menu/system_engine1.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_engine2.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_engine3.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_engine4.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_mechan1.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_mechan2.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_mechan3.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_mechan4.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_power1.tga",				0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_power2.tga",				0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_power3.tga",				0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_power4.tga",				0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_protect1.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_protect2.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_protect3.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_protect4.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_target1.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_target2.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_target3.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_target4.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/system_empty.tga",				0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
// иконки оружия для передаскивания-отображения в слотах оружия
{"menu/weapon1_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon2_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon3_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon4_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon5_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon6_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon7_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon8_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon9_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon10_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon11_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon12_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon13_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon14_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon15_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon16_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon17_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon18_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon19_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon_on_icon.tga",		0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon_empty_icon.tga",		0, 256, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
// курсор
{"menu/cursor.tga",				0, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/cursor_shadow.tga",			0, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
// текстура брони для кораблей землян
{"models/earthfighter/sf-text00.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-text05.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-text06.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-text07.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-text08.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-text09.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-text04.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-text10.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-illum01.vw2d",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-illum02.vw2d",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-illum03.vw2d",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-illum04.vw2d",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/lnch12.tga",			1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/lnch34.tga",			1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/rockets.tga",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// текстура истребителей пришельцев
{"models/alienfighter/al-text04.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/alienfighter/al-illum04.vw2d",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// текстура космических объектов
{"models/space/asteroid-01.tga",			1, 96, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// планеты
{"models/planet/asteroid.tga",			1, 512, true, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/planet/clouds.tga",			1, 1024, true, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, false, -1.0f, false},
{"models/planet/q_class2.tga",				1, 192, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/planet/a_class4.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/planet/m_class7.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/planet/d_class3.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/m_class7_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/d_class3_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/a_class4_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/planet_asteroids_nm.tga",	1, 384, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/q_class2_nm.tga",		1, 192, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// текстура наземной техники-сооружений
{"models/gr-01.vw2d",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/gr-02.vw2d",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/gr-03.vw2d",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/gr-04.vw2d",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/bomber_nm.tga",				1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/gr-05.vw2d",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/gr-06.vw2d",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/gr-07.vw2d",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/track.vw2d",							1, 48, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},

// текстура больших кораблей пришельцев
{"models/alienmothership/alm-text02.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/alienmothership/alm-text03.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/alienmothership/alm-text04.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/alienmothership/alm-text08.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/alienmothership/alm-illum02.vw2d",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/alienmothership/alm-illum03.vw2d",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/alienmothership/alm-illum04.vw2d",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/alienmothership/alm-illum08.vw2d",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/alien_mothership_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// турели пиратов
{"models/turret/turrets.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// текстура зданий
{"models/building/bld.vw2d",				1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/buildings_nm.tga",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/building/bld_illum.vw2d",			1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// космическая база
{"models/spacebase/allalpha.tga",		1, 1024, true, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/spacebase/metal.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, false, -1.0f, false},
// мины
{"models/mine/mine1.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/mine/mine1i.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/mine/mine2.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/mine/mine2i.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/mine/mine3.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/mine/mine3i.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/mine/mine4.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/mine/mine4i.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},

// скайбокс - космос, загружаем только те, что нужно - экономим видео память
{"skybox/1/skybox_bottom4.tga",			1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"skybox/1/skybox_front5.tga",				1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"skybox/tile_back.tga",					1, 4096/2, true,  0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"skybox/tile_stars.tga",					1, 4096/2, true,  0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// спец эффекты
{"gfx/flare.tga",							1, 16, true,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"gfx/flare1.tga",							1, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"gfx/flare2.tga",							1, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"gfx/flare3.tga",							1, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"gfx/trail1.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"gfx/trail2.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"gfx/trail3.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"gfx/trail4.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"gfx/trail5.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},

};
#define MenuLoadListCount sizeof(MenuLoadList)/sizeof(MenuLoadList[0])
















static LoadList GameLevelsLoadList[] =
{
// 2д часть
{"menu/cursor.tga",						0, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/cursor_shadow.tga",					0, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/blackpoint.tga",					0, 0, true, 255,255,255, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/dialog512_256.tga",					0, 706, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button_dialog200_out.tga",			0, 57, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button_dialog200_in.tga",			0, 57, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button_dialog200_off.tga",			0, 57, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button_dialog128_out.tga",			0, 39, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button_dialog128_in.tga",			0, 39, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button_dialog128_off.tga",			0, 39, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button384_back.tga",				0, 192, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button384_in.tga",					0, 96, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/button384_out.tga",					0, 96, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/dialog512_512.tga",					0, 1242, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"menu/perc.tga",							0, 2, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/perc_none.tga",						0, 2, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"game/nums.tga",							0, 104, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"game/ammo.tga",							0, 2, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"game/energy.tga",						0, 2, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"game/game_panel.tga",					0, 296, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"game/game_panel2.tga",					0, 347, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"game/game_panel_el.tga",					0, 256, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"game/game_num.tga",						0, 31, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"game/weapon_panel_left.tga",				0, 56, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"game/weapon_panel_right.tga",			0, 56, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"game/weapon_ammo.tga",					0, 1, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"game/weapon_energy.tga",					0, 1, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon_on_icon.tga",				0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"lang/en/game/mission.tga",					0, 64, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"lang/en/game/missionfailed.tga",				0, 168, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"lang/en/game/pause.tga",						0, 64, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"lang/de/game/mission.tga",					0, 64, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"lang/de/game/missionfailed.tga",				0, 168, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"lang/de/game/pause.tga",						0, 64, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"lang/ru/game/mission.tga",					0, 64, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"lang/ru/game/missionfailed.tga",				0, 168, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"lang/ru/game/pause.tga",						0, 64, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"skybox/tile_back.tga",					1, 4096/2, true,  0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"skybox/tile_stars.tga",					1, 4096/2, true,  0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// спец эффекты
{"gfx/flare.tga",							1, 16, true,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"gfx/flare1.tga",							1, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"gfx/flare2.tga",							1, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"gfx/flare3.tga",							1, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"gfx/trail1.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"gfx/trail2.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"gfx/trail3.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"gfx/trail4.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"gfx/trail5.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
// иконки оружия для перетаскивания-отображения в слотах оружия
{"menu/weapon1_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon2_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon3_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon4_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon5_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon6_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon7_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon8_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon9_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon10_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon11_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon12_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon13_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon14_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon15_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon16_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon17_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon18_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"menu/weapon19_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
// текстура брони для кораблей землян
{"models/earthfighter/sf-text00.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-text04.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-text05.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-text06.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-text07.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-text08.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-text09.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-text10.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-illum01.vw2d",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-illum02.vw2d",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-illum03.vw2d",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/sf-illum04.vw2d",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/lnch12.tga",			1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/lnch34.tga",			1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/earthfighter/rockets.tga",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// звуки игры (только игры), грузим тут, иначе плохо, когда подгружает во время игры
{"sfx/weapon1probl.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weapon2probl.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weapon3probl.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weapon4probl.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weapon5probl.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/explosion1.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/explosion2.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/explosion3.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/explosion4.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire1.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire2.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire3.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire4.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire5.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire6.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire7.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire8.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire9.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire10.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire11.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire12.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire13.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire14.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire15.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire16.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire17.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire18.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/weaponfire19.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/kinetichit.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/ionhit.wav",							4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/plasmahit.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/antimaterhit.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/gausshit.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"sfx/lowlife.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
// мины
{"models/mine/mine1.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/mine/mine1i.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/mine/mine2.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/mine/mine2i.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/mine/mine3.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/mine/mine3i.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/mine/mine4.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/mine/mine4i.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// модели мин
{"models/mine/mine-01.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/mine/mine-02.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/mine/mine-03.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/mine/mine-04.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
// корабли землян + их оружие
{"models/earthfighter/sf-01.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-02.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-03.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-04.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-05.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-06.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-07.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-08.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-09.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-10.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-11.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-12.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-13.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-14.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-15.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-16.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-17.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-18.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-19.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-20.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-21.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/sf-22.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/weapons.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/lnch1.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/lnch2.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/lnch3.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/lnch4.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/missile.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/swarm.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/torpedo.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/earthfighter/nuke.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
};
#define GameLevelsLoadListCount sizeof(GameLevelsLoadList)/sizeof(GameLevelsLoadList[0])



static LoadList AlienFighterLoadList[] =
{
// AlienFighter – load alien fighters textures.
{"models/alienfighter/al-text04.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/alienfighter/al-illum04.vw2d",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// малые корабли пришельцев
{"models/alienfighter/al-01.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/alienfighter/al-02.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/alienfighter/al-03.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/alienfighter/al-04.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/alienfighter/al-05.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/alienfighter/al-06.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/alienfighter/al-07.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/alienfighter/al-08.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/alienfighter/al-09.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/alienfighter/al-10.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/alienfighter/al-11.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/alienfighter/al-12.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/alienfighter/al-13.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/alienfighter/al-14.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/alienfighter/al-15.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/alienfighter/al-16.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/alienfighter/al-17.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
};
#define AlienFighterLoadListCount sizeof(AlienFighterLoadList)/sizeof(AlienFighterLoadList[0])




static LoadList PirateLoadList[] =
{
// Pirate – load all pirate data (vehicles, military buildings, ships...) testures.
{"models/gr-01.vw2d",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/gr-02.vw2d",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/gr-03.vw2d",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/gr-04.vw2d",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/bomber_nm.tga",				1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/gr-05.vw2d",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/gr-06.vw2d",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/gr-07.vw2d",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/track.vw2d",							1, 48, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/turret/turrets.tga",					1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// корабли пиратов
{"models/pirateship/gunship-01.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/pirateship/bomber-07.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/pirateship/gunship-03.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/pirateship/gunship-04.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/pirateship/gunship-02.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/pirateship/bomber-03.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/pirateship/bomber-02.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/pirateship/bomber-04.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/pirateship/bomber-05.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
{"models/pirateship/bomber-06.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
// турелей
{"models/turret/turret-01.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/turret/turret-02.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
// гусенечный транспорт
{"models/tracked/engineering-01.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/tracked/apc-aa-02.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/tracked/apc-aa-01.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/tracked/apc-03.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/tracked/apc-01.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/tracked/tank-11.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/tracked/tank-10.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/tracked/tank-09.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/tracked/tank-08.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/tracked/tank-07.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/tracked/tank-06.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/tracked/tank-05.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/tracked/tank-03.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/tracked/tank-01.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
// колесный транспорт
{"models/wheeled/r-launcher-01.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/wheeled/apc-04.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/wheeled/apc-02.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/wheeled/jeep-05.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/wheeled/jeep-04.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/wheeled/jeep-03.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/wheeled/jeep-02.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/wheeled/jeep-01.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
// военные сооружения
{"models/militarybuilding/artiler-gun-02.vw3d",2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/militarybuilding/artiler-gun-01.vw3d",2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/militarybuilding/aa-gun-05.vw3d",		2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/militarybuilding/aa-gun-04.vw3d",		2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/militarybuilding/aa-gun-03.vw3d",		2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/militarybuilding/aa-gun-02.vw3d",		2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"models/militarybuilding/aa-gun-01.vw3d",		2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
};
#define PirateLoadListCount sizeof(PirateLoadList)/sizeof(PirateLoadList[0])



static LoadList BasePartLoadList[] =
{
// BasePart – load pirate base textures.
{"models/spacebase/allalpha.tga",		1, 768, true, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/spacebase/metal.tga",			1, 1024, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, false, -1.0f, false},
{"models/planet/d_class3.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/d_class3_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// геометрия базы пиратов
{"models/spacebase/1/1.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/1/2.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/1/3.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/1/4.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/1/5.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/2/1.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/2/2.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/2/3.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/2/4.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/2/5.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/3/1.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/spacebase/4/1.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/spacebase/5/1.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/spacebase/6/1.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/6/2.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/6/3.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/6/4.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/6/5.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/7/1.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/spacebase/8/1.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/8/2.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/8/3.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/8/4.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/spacebase/8/5.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
};
#define BasePartLoadListCount sizeof(BasePartLoadList)/sizeof(BasePartLoadList[0])



static LoadList AsteroidLoadList[] =
{
// Asteroid – load asteroids (for AsteroidField) textures.
{"models/space/asteroid-01.tga",				1, 96, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// большие астероиды
{"models/planet/d_class3.tga",					1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/d_class3_nm.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// геометрия больших астероидов
{"models/space/bigasteroid-01.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/space/bigasteroid-02.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/space/bigasteroid-03.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/space/bigasteroid-04.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/space/bigasteroid-05.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
// астероиды
{"models/space/asteroid-010.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-011.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-012.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-013.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-014.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-015.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-016.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-017.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-018.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-019.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-0110.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-0111.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-0112.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-0113.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-0114.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-0115.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-0116.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-0117.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-0118.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"models/space/asteroid-0119.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
};
#define AsteroidLoadListCount sizeof(AsteroidLoadList)/sizeof(AsteroidLoadList[0])



static LoadList PlanetLoadList[] =
{
// Planet – load planets textures.
{"models/planet/asteroid.tga",			1, 512, true, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/planet/clouds.tga",			1, 1024, true, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, false, -1.0f, false},
{"models/planet/q_class2.tga",				1, 192, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/planet/a_class4.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/planet/m_class7.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/planet/d_class3.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/m_class7_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/d_class3_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/a_class4_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/planet_asteroids_nm.tga",	1, 384, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/q_class2_nm.tga",		1, 192, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// модели планет
{"models/planet/aplanet.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/planet/dplanet.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/planet/gplanet.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/planet/moon.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/planet/planet5.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/planet/planet6.vw3d",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
};
#define PlanetLoadListCount sizeof(PlanetLoadList)/sizeof(PlanetLoadList[0])



static LoadList AlienMotherShipLoadList[] =
{
// AlienMotherShip – load alien motherships textures.
{"models/alienmothership/alm-text02.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/alienmothership/alm-text03.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/alienmothership/alm-text04.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/alienmothership/alm-text08.vw2d",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/alienmothership/alm-illum02.vw2d",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/alienmothership/alm-illum03.vw2d",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/alienmothership/alm-illum04.vw2d",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/alienmothership/alm-illum08.vw2d",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/alien_mothership_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// большие корабли пришельцев
{"models/alienmothership/alm-01.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
{"models/alienmothership/alm-02.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
{"models/alienmothership/alm-03.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
{"models/alienmothership/alm-04.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
{"models/alienmothership/alm-05.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
{"models/alienmothership/alm-06.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
{"models/alienmothership/alm-07.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
{"models/alienmothership/alm-08.vw3d",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
};
#define AlienMotherShipLoadListCount sizeof(AlienMotherShipLoadList)/sizeof(AlienMotherShipLoadList[0])



static LoadList BuildingLoadList[] =
{
// Building – load buildings textures.
{"models/building/bld.vw2d",				1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/normalmap/buildings_nm.tga",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"models/building/bld_illum.vw2d",			1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// постройки (мирные)
{"models/building/bld-01.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/building/bld-02.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/building/bld-03.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/building/bld-04.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/building/bld-05.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/building/bld-06.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/building/bld-07.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/building/bld-08.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/building/bld-09.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/building/bld-10.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"models/building/bld-11.vw3d",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
};
#define BuildingLoadListCount sizeof(BuildingLoadList)/sizeof(BuildingLoadList[0])



static LoadList StarSystem1LoadList[] =
{
// StarSystem1 – load StarSystem 1 SkyBox textures.
{"skybox/1/skybox_back6.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"skybox/1/skybox_bottom4.tga",				1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"skybox/1/skybox_front5.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"skybox/1/skybox_left2.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"skybox/1/skybox_right1.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"skybox/1/skybox_top3.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
};
#define StarSystem1LoadListCount sizeof(StarSystem1LoadList)/sizeof(StarSystem1LoadList[0])



static LoadList StarSystem2LoadList[] =
{
//StarSystem2 – load StarSystem 2 SkyBox textures.
{"skybox/2/skybox_back6.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"skybox/2/skybox_bottom4.tga",				1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"skybox/2/skybox_front5.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"skybox/2/skybox_left2.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"skybox/2/skybox_right1.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"skybox/2/skybox_top3.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
};
#define StarSystem2LoadListCount sizeof(StarSystem2LoadList)/sizeof(StarSystem2LoadList[0])
















//------------------------------------------------------------------------------------
// процедура прорисовки логотипа
//------------------------------------------------------------------------------------
void DrawViewizardLogo(eTexture *ViewizardLogoTexture)
{
	int		ShowLogoTime = 6000; // сколько нужно показывать логотип
	int		ShowLogoLife = ShowLogoTime; // сколько осталось показывать
	Uint32	ShowLogoPrevTime = SDL_GetTicks();

	vw_SetClearColor(1.0f,1.0f,1.0f,1.0f);

	while (ShowLogoLife > 0)
	{
		RECT SrcRect,DstRect;
		SetRect(&SrcRect, 1,1,511,511);
		int StartX = (Setup.iAspectRatioWidth-510)/2;
		int EndX = StartX+510;
		SetRect(&DstRect, StartX, 128+1, EndX, 640-2);
		float Transp = 1.0f;
		float GreyColor = 1.0f;

		// плавно делаем появление
		if (ShowLogoLife > ShowLogoTime/2.0f)
		{
			Transp = (ShowLogoTime/2.0f - (ShowLogoLife-ShowLogoTime/2.0f))/(ShowLogoTime/2.0f);
		}
		else
		{
			Transp = 1.0f;
			// относительно быстро исчезаем
			if (ShowLogoLife < ShowLogoTime/4.0f)
			{
				GreyColor = ShowLogoLife/(ShowLogoTime/4.0f);
				vw_SetClearColor(GreyColor,GreyColor,GreyColor,1.0f);
			}
		}
		Transp-=.01f; // чтобы всегда был немного прозрачным
		if (Transp < 0.0f) Transp = 0.0f;


		// рисуем
		vw_BeginRendering(RI_COLOR_BUFFER | RI_DEPTH_BUFFER);
		vw_Start2DMode(-1,1);

		vw_DrawTransparent(&DstRect, &SrcRect, ViewizardLogoTexture, true, Transp, 0.0f, RI_UL_CORNER, GreyColor, GreyColor, GreyColor);

		vw_End2DMode();
		vw_EndRendering();


		// проверка времени
		ShowLogoLife -= SDL_GetTicks() - ShowLogoPrevTime;
		ShowLogoPrevTime = SDL_GetTicks();
		if (ShowLogoLife <= 0) ShowLogoLife = 0;


		SDL_Event event;
		while ( SDL_PollEvent(&event) )
		{
			switch (event.type)
			{
				case SDL_MOUSEBUTTONDOWN:
				case SDL_KEYDOWN:
				case SDL_JOYBUTTONDOWN:
						ShowLogoLife = 0;
					break;
				default:
					break;
			}
		}

		SDL_Delay(2);

#ifndef multithread
		// ставим и сюда, иначе не сможем играть во время загрузки
		Audio_LoopProc();
#endif
	}


	vw_SetClearColor(0.0f,0.0f,0.0f,1.0f);
}











//------------------------------------------------------------------------------------
// процедура прорисовки процента загрузки данных
//------------------------------------------------------------------------------------
void DrawLoading(int Current, int AllDrawLoading, float *LastDrawTime, eTexture *LoadImageTexture)
{
	// слишком часто не рисуем
	if (Current != AllDrawLoading) // последний (полный) рисуем всегда
		if ((*LastDrawTime) + 0.035 >= vw_GetTime()) return;

	vw_BeginRendering(RI_COLOR_BUFFER | RI_DEPTH_BUFFER);
	vw_Start2DMode(-1,1);

	RECT SrcRect, DstRect;

	// выводим картинку
	SetRect(&SrcRect, 0,0,1024,512);
	SetRect(&DstRect, 0, 64+32,Setup.iAspectRatioWidth,64+32+512);
	vw_DrawTransparent(&DstRect, &SrcRect, LoadImageTexture, false, 1.0f, 0.0f);

	// пишем "загрузка"
	vw_DrawFont(Setup.iAspectRatioWidth/2-vw_FontSize(vw_GetText("11_Loading"))/2, 768-128, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 1.0f, vw_GetText("11_Loading"));

	// выводим подложку линии загрузки
	SetRect(&SrcRect, 0,0,256,32);
	int StartX = (Setup.iAspectRatioWidth-256)/2;
	vw_Draw(StartX, 768-64-8 -32, &SrcRect, vw_FindTextureByName("loading/loading_back.tga"), true);

	// выводим линию загрузки
	int loaded = (int)(256.0f*Current/AllDrawLoading);
	SetRect(&SrcRect, 0,0,loaded,16);
	vw_Draw(StartX, 768-64-1 -32, &SrcRect, vw_FindTextureByName("loading/loading_line.tga"), true);


	// выводим хинт при загрузке
	if (NeedShowHint)
	{
		int	Size = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText(LoadingHints[Setup.LoadingHint])))/2;
		vw_DrawFont(Size, 740, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.99f, vw_GetText(LoadingHints[Setup.LoadingHint]));
	}

	vw_End2DMode();
	vw_EndRendering();


	// обработчик окна
	SDL_Event event;
	while ( SDL_PollEvent(&event) )
	{
		switch (event.type)
		{
			case SDL_QUIT:
				Quit = true;
				break;
			default:
				break;
		}
	}

#ifndef multithread
	// ставим и сюда, иначе не сможем играть во время загрузки
	Audio_LoopProc();
#endif

	(*LastDrawTime) = vw_GetTime();
}







//------------------------------------------------------------------------------------
// процедура освобождения данных, что удалять определяем по типу загрузки
//------------------------------------------------------------------------------------
int CurretnLoadedData = -2;
bool ReleaseGameData(int LoadType)
{
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// сбрасываем данные глобальных переменных
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	CurrentList = 0;
	CurrentListCount = 0;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// удаляем данные из памяти
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ReleaseAllObject3D();
	vw_ReleaseAllParticleSystems();
	ReleaseAllGameLvlText();
	vw_ReleaseAllLights();
	StarSystemRelease();

	// если это не переход меню-игра, снимаем звук
	vw_ReleaseAllSounds(1);

	// если не менее 128 мб видео памяти - выгружать текстуры вообще не нужно
	if (!Setup.EqualOrMore128MBVideoRAM)
	{
		// если выбрали миссию, и была миссия (т.е. рестарт миссии) - ничего не удаляем-чистим
		if (CurretnLoadedData == LoadType && CurretnLoadedData == 1)
		{
			// эту миссию уже загружали
			CurretnLoadedData = LoadType;
			return true;
		}
		else
		{
			vw_ReleaseAllFontChars(); // (!) всегда перед vw_ReleaseAllTextures
			vw_ReleaseAllTextures();
			CurretnLoadedData = LoadType;
			return false;
		}
	}
	else
	{
		// нужно понять, мы конкретно это загружали или нет
		if (LoadType <= 0) // это меню
		{
			if (LoadedTypes[0]) return true;
			else
			{
				LoadedTypes[0] = true;
				return false;
			}
		}
		else // это миссия
		{
			if (CurrentMission >= 0)
			{
				if (LoadedTypes[CurrentMission+1]) return true;
				else
				{
					LoadedTypes[CurrentMission+1] = true;
					return false;
				}
			}
		}
	}

	return false;
}







// отдельный поток для проигывания музыки при загрузке
#ifdef multithread

//------------------------------------------------------------------------------------
// Процедура звука
//------------------------------------------------------------------------------------
bool LoadSoundThreadNeedOff = false;
int LoadSoundThread(void *UNUSED(data))
{
	LoadSoundThreadNeedOff = false;

	while (!LoadSoundThreadNeedOff)
	{
		Audio_LoopProc();
		SDL_Delay(10);
	}

	return 0;
}

#endif //multithread










//------------------------------------------------------------------------------------
// процедура загрузки данных, тип загрузки, с логотипом (-1) или без (0-10)
//------------------------------------------------------------------------------------
void LoadGameData(int LoadType)
{

	// потом передавать в функцию тип загрузки:
	// -1 - загрузка меню с логотипом
	// 0 - загрузка меню без логотипа
	// 1...10 - загрузка 1...10 уровня


	eTexture *LoadImageTexture = 0;
	int RealLoadedTextures = 0;
	bool NeedLoadShaders = false;
	int AllDrawLoading = 0;
#ifdef multithread
	SDL_Thread *SoundThread = 0;
#endif //multithread

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверяем, если уже что-то было загружено, если данные для этой миссии-меню загружены - тут вообще нечего делать
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool NeedStartGoto = false;
	if (ReleaseGameData(LoadType)) NeedStartGoto = true;


	// ставим время последней прорисовки
	vw_StartTime();
	float LastDrawTime = vw_GetTime();


	NeedLoadShaders = false;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// подключаем список загрузки
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	switch(LoadType)
	{
		// меню, загрузка в самом начале
		case -1:
			CurrentList = MenuLoadList;
			CurrentListCount = MenuLoadListCount;
			NeedShowHint = false;
			break;
		// переход игра-меню
		case 0:
			SaveXMLSetupFile();
			CurrentList = MenuLoadList;
			CurrentListCount = MenuLoadListCount;
			NeedShowHint = true;
			break;

		// уровни игры
		case 1:
		{
			SaveXMLSetupFile();
			CurrentListCount = GameLevelsLoadListCount;
			NeedShowHint = true;

			// флаги нужно загружать или нет...
			bool StarSystem1 = false;
			bool StarSystem2 = false;
			bool Planet = false;
			bool Asteroid = false;
			bool AlienFighter = false;
			bool BasePart = false;
			bool AlienMotherShip = false;
			bool Building = false;
			bool Pirate = false;

			// по скрипту, смотрим что загружать + считаем сколько позиций
			char *FileName = GetMissionFileName();
			if (FileName == 0)
			{
				fprintf(stderr, "Critical error. Can't find this mission script file or mission list file.\n");
				fprintf(stderr, "Please, check your ./script/ folder for xml files (aimode, list, missions).\n");
				exit(0);
			}

			cXMLDocument *xmlDoc = new cXMLDocument;


			// читаем данные
			if (!xmlDoc->Load(FileName))
			{
				fprintf(stderr, "Can't find script file or file corrupted: %s\n", FileName);
				delete xmlDoc;
				exit(0);
			}

			// проверяем корневой элемент
			if (strcmp("AstroMenaceScript", xmlDoc->RootXMLEntry->Name))
			{
				fprintf(stderr, "Can't find AstroMenaceScript element in the: %s\n", FileName);
				delete xmlDoc;
				exit(0);
			}

			// переходим на загрузку
			cXMLEntry *xmlEntry = xmlDoc->FindFirstChildEntryByName(xmlDoc->RootXMLEntry, "Load");
			if (xmlEntry == 0)
			{
				fprintf(stderr, "Can't find Load element in the: %s\n", FileName);
				delete xmlDoc;
				exit(0);
			}

			xmlEntry = xmlEntry->FirstChild;
			if (xmlEntry == 0)
			{
				fprintf(stderr, "Can't find Load element in the: %s\n", FileName);
				delete xmlDoc;
				exit(0);
			}

			// установка прозрачности слоев
			StarsTileStartTransparentLayer1 = 0.2f;
			StarsTileEndTransparentLayer1 = 0.7f;
			StarsTileStartTransparentLayer2 = 0.9f;
			StarsTileEndTransparentLayer2 = 0.7f;

			while (xmlEntry)
			{
				if (!strcmp(xmlEntry->Name, "StarSystem1")) StarSystem1 = true;
				else
				if (!strcmp(xmlEntry->Name, "StarSystem2")) StarSystem2 = true;
				else
				if (!strcmp(xmlEntry->Name, "Planet")) Planet = true;
				else
				if (!strcmp(xmlEntry->Name, "Asteroid")) Asteroid = true;
				else
				if (!strcmp(xmlEntry->Name, "AlienFighter")) AlienFighter = true;
				else
				if (!strcmp(xmlEntry->Name, "BasePart")) BasePart = true;
				else
				if (!strcmp(xmlEntry->Name, "AlienMotherShip")) AlienMotherShip = true;
				else
				if (!strcmp(xmlEntry->Name, "Building")) Building = true;
				else
				if (!strcmp(xmlEntry->Name, "Pirate")) Pirate = true;
				else
				if (!strcmp(xmlEntry->Name, "AIFile")) // загружаем данные по AI
				{
					if (strlen(xmlEntry->Content) > 0)
						InitGameAI(xmlEntry->Content); // "script/aimode.xml"
				}
				else
				if (!strcmp(xmlEntry->Name, "LayersTransp"))
				{
					if (xmlDoc->GetEntryAttribute(xmlEntry, "FirstStart") != 0)
							StarsTileStartTransparentLayer1 = xmlDoc->fGetEntryAttribute(xmlEntry, "FirstStart");
					if (xmlDoc->GetEntryAttribute(xmlEntry, "FirstEnd") != 0)
							StarsTileEndTransparentLayer1 = xmlDoc->fGetEntryAttribute(xmlEntry, "FirstEnd");
					if (xmlDoc->GetEntryAttribute(xmlEntry, "SecondStart") != 0)
							StarsTileStartTransparentLayer2 = xmlDoc->fGetEntryAttribute(xmlEntry, "SecondStart");
					if (xmlDoc->GetEntryAttribute(xmlEntry, "SecondEnd") != 0)
							StarsTileStartTransparentLayer2 = xmlDoc->fGetEntryAttribute(xmlEntry, "SecondEnd");
				}

				// берем следующий элемент по порядку
				xmlEntry = xmlEntry->Next;
			}

			// чистим память, со скриптом работать больше не надо
			delete xmlDoc;


			// считаем сколько там элементов
			if (StarSystem1) CurrentListCount += StarSystem1LoadListCount;
			if (StarSystem2) CurrentListCount += StarSystem2LoadListCount;
			if (Planet) CurrentListCount += PlanetLoadListCount;
			if (Asteroid) CurrentListCount += AsteroidLoadListCount;
			if (AlienFighter) CurrentListCount += AlienFighterLoadListCount;
			if (BasePart) CurrentListCount += BasePartLoadListCount;
			if (AlienMotherShip) CurrentListCount += AlienMotherShipLoadListCount;
			if (Building) CurrentListCount += BuildingLoadListCount;
			if (Pirate) CurrentListCount += PirateLoadListCount;

			// выделяем память
			CurrentList = new LoadList[CurrentListCount];

			// составляем список загрузки

			// копируем основную часть
			int Current = 0;
			memcpy(CurrentList+Current, GameLevelsLoadList, GameLevelsLoadListCount*sizeof(LoadList));
			Current += GameLevelsLoadListCount;

			if (StarSystem1)
			{
				memcpy(CurrentList+Current, StarSystem1LoadList, StarSystem1LoadListCount*sizeof(LoadList));
				Current += StarSystem1LoadListCount;
			}
			if (StarSystem2)
			{
				memcpy(CurrentList+Current, StarSystem2LoadList, StarSystem2LoadListCount*sizeof(LoadList));
				Current += StarSystem2LoadListCount;
			}

			if (Planet)
			{
				memcpy(CurrentList+Current, PlanetLoadList, PlanetLoadListCount*sizeof(LoadList));
				Current += PlanetLoadListCount;
			}
			if (Asteroid)
			{
				memcpy(CurrentList+Current, AsteroidLoadList, AsteroidLoadListCount*sizeof(LoadList));
				Current += AsteroidLoadListCount;
			}
			if (AlienFighter)
			{
				memcpy(CurrentList+Current, AlienFighterLoadList, AlienFighterLoadListCount*sizeof(LoadList));
				Current += AlienFighterLoadListCount;
			}
			if (BasePart)
			{
				memcpy(CurrentList+Current, BasePartLoadList, BasePartLoadListCount*sizeof(LoadList));
				Current += BasePartLoadListCount;
			}
			if (AlienMotherShip)
			{
				memcpy(CurrentList+Current, AlienMotherShipLoadList, AlienMotherShipLoadListCount*sizeof(LoadList));
				Current += AlienMotherShipLoadListCount;
			}
			if (Building)
			{
				memcpy(CurrentList+Current, BuildingLoadList, BuildingLoadListCount*sizeof(LoadList));
				Current += BuildingLoadListCount;
			}
			if (Pirate)
			{
				memcpy(CurrentList+Current, PirateLoadList, PirateLoadListCount*sizeof(LoadList));
				Current += PirateLoadListCount;
			}
		}
			break;
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// включаем музыку
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	switch(LoadType)
	{
		// меню, загрузка в самом начале
		case -1:
			GameStatus = MAIN_MENU;
			Audio_LoopProc();
			StartMusicWithFade(0, 4.0f, 4.0f);
			break;
		// переход игра-меню
		case 0:
			GameStatus = MISSION;
			StartMusicWithFade(0, 2.0f, 2.0f);
			break;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// идем на переход именно отсюда, иначе не подключим файл с AI
	// и надо запустить нужную музыку
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (NeedStartGoto) goto AllDataLoaded;




	AllDrawLoading = 0;
	// получаем значение (реальное, по весам)
	for (unsigned int i=0; i<CurrentListCount; i++)
	{
		AllDrawLoading += CurrentList[i].Value;
	}



	// если будем загружать шейдеры - делаем поправку общего кол-ва
	if (vw_GetDevCaps()->GLSL100Supported && Setup.UseGLSL)
	{
		// там нет ни одного
		if (vw_FindShaderByNum(1) == 0)
		{
			AllDrawLoading += GLSLLoadListCount*100;
			NeedLoadShaders = true;
		}
	}


	// загружаем все по списку
	RealLoadedTextures = 0;



#ifdef multithread
	// поток проигрывания звука
#ifdef use_SDL2
	SoundThread = SDL_CreateThread(LoadSoundThread, "MusicThread", 0);
#else
	SoundThread = SDL_CreateThread(LoadSoundThread, 0);
#endif // use_SDL2
#endif //multithread




	// в самом начале () до прорисовки подложки загрузки - генерируем все возможные символы для меню (чтобы в процессе прорисовки меньше подгружать)
	// если памяти мало, мы очищаем текстуры, надо перегенерировать шрифт и создать новые текстуры
	if ((LoadType == -1) || (!Setup.EqualOrMore128MBVideoRAM))
	{
		// задаем размеры текстуры (всегда степерь 2 ставим, чтобы избежать проблем со старым железом)
		vw_GenerateFontChars(Setup.FontSize > 16 ? 512 : 256, 256, " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+\():;%&`'*#$=[]@^{}_~><–—«»“”|абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЧЦШЩЪЫЬЭЮЯ©®ÄÖÜäöüß°§/");
#ifdef gamedebug
		// проверяем все ли символы из текущего языкового файла вошли в прегенерацию, иначе не сможем потом рисовать меню через одну текстуру
		// смысла гонять постоянно такую проверку нет, один раз сводим все символы языка и не замедляем загрузку поиском
		// + есть часть символов прописанных в коде, так что убирать англ и часть символов нельзя (!)
		vw_CheckFontCharsInText();
#endif // gamedebug
	}




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// загружаем логотип компании
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (LoadType == -1)
	{
		// выводим логотип Viewizard
		vw_SetTextureProp(TEXTURE_NO_MIPMAP, RI_CLAMP_TO_EDGE, false, TX_ALPHA_EQUAL, false);
		eTexture *ViewizardLogoTexture = vw_LoadTexture("loading/viewizardlogo.tga", NULL, 0);

		DrawViewizardLogo(ViewizardLogoTexture);

		vw_ReleaseTexture(ViewizardLogoTexture);
	}




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// загружаем список
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// загружаем картинки вывода загрузки
	vw_SetTextureAlpha(0, 0, 0);
	vw_SetTextureProp(TEXTURE_NO_MIPMAP, RI_CLAMP_TO_EDGE, true, TX_ALPHA_GREYSC, false);
	vw_LoadTexture("loading/loading_line.tga", NULL, 0);
	vw_LoadTexture("loading/loading_back.tga", NULL, 0);
	vw_SetTextureProp(TEXTURE_NO_MIPMAP, RI_CLAMP_TO_EDGE, false, TX_ALPHA_GREYSC, false);

	LoadImageTexture = 0;

	switch (1+vw_iRandNum(3))
	{
		case 1:
			LoadImageTexture = vw_LoadTexture("loading/loading01.tga", NULL, 0);
			break;
		case 2:
			LoadImageTexture = vw_LoadTexture("loading/loading02.tga", NULL, 0);
			break;
		case 3:
			LoadImageTexture = vw_LoadTexture("loading/loading03.tga", NULL, 0);
			break;
		case 4:
			LoadImageTexture = vw_LoadTexture("loading/loading04.tga", NULL, 0);
			break;
		// на всякий случай
		default:
			LoadImageTexture = vw_LoadTexture("loading/loading01.tga", NULL, 0);
			break;
	}



	//	если нужно, загрузка всех шейдеров (!) обязательно это делать до загрузки моделей
	if (NeedLoadShaders)
	if (vw_GetDevCaps()->GLSL100Supported)
	{
		for (unsigned int i=0; i<GLSLLoadListCount; i++)
		if (Setup.UseGLSL)
		{

			eGLSL *Program = 0;
			Program = vw_CreateShader(GLSLLoadList[i].Name, GLSLLoadList[i].VertexShaderFileName, GLSLLoadList[i].FragmentShaderFileName);

			if (Program != 0)
			{
				// получаем сразу состояние, смогли прилинковать или нет
				if (!vw_LinkShaderProgram(Program)) Setup.UseGLSL = false;
			}
			else
				Setup.UseGLSL = false;

			RealLoadedTextures += 1000;
			// рисуем текущее состояние загрузки, если не рисуем логотип
			DrawLoading(RealLoadedTextures, AllDrawLoading, &LastDrawTime, LoadImageTexture);
		}

		// сразу находим базовые типы шейдеров для прорисовки 3д моделей
		GLSLShaderType1 = vw_FindShaderByName("PerPixelLight");
		GLSLShaderType2 = vw_FindShaderByName("PerPixelLight_Explosion");
		GLSLShaderType3 = vw_FindShaderByName("PerPixelLight_ShadowMap");

		// находим все юниформы GLSLShaderType1
		UniformLocations[0] = vw_GetUniformLocation(GLSLShaderType1, "Texture1");
		UniformLocations[1] = vw_GetUniformLocation(GLSLShaderType1, "Texture2");
		UniformLocations[2] = vw_GetUniformLocation(GLSLShaderType1, "DirectLightCount");
		UniformLocations[3] = vw_GetUniformLocation(GLSLShaderType1, "PointLightCount");
		UniformLocations[4] = vw_GetUniformLocation(GLSLShaderType1, "NeedMultitexture");
		UniformLocations[5] = vw_GetUniformLocation(GLSLShaderType1, "NormalMap");
		UniformLocations[6] = vw_GetUniformLocation(GLSLShaderType1, "NeedNormalMapping");
		// находим все юниформы GLSLShaderType2
		UniformLocations[10] = vw_GetUniformLocation(GLSLShaderType2, "Texture1");
		UniformLocations[11] = vw_GetUniformLocation(GLSLShaderType2, "DirectLightCount");
		UniformLocations[12] = vw_GetUniformLocation(GLSLShaderType2, "PointLightCount");
		UniformLocations[13] = vw_GetUniformLocation(GLSLShaderType2, "SpeedData1");
		UniformLocations[14] = vw_GetUniformLocation(GLSLShaderType2, "SpeedData2");
		// находим все юниформы GLSLShaderType3
		UniformLocations[20] = vw_GetUniformLocation(GLSLShaderType3, "Texture1");
		UniformLocations[21] = vw_GetUniformLocation(GLSLShaderType3, "Texture2");
		UniformLocations[22] = vw_GetUniformLocation(GLSLShaderType3, "DirectLightCount");
		UniformLocations[23] = vw_GetUniformLocation(GLSLShaderType3, "PointLightCount");
		UniformLocations[24] = vw_GetUniformLocation(GLSLShaderType3, "NeedMultitexture");
		UniformLocations[25] = vw_GetUniformLocation(GLSLShaderType3, "ShadowMap");
		UniformLocations[26] = vw_GetUniformLocation(GLSLShaderType3, "xPixelOffset");
		UniformLocations[27] = vw_GetUniformLocation(GLSLShaderType3, "yPixelOffset");
		UniformLocations[28] = vw_GetUniformLocation(GLSLShaderType3, "NormalMap");
		UniformLocations[29] = vw_GetUniformLocation(GLSLShaderType3, "NeedNormalMapping");
		UniformLocations[30] = vw_GetUniformLocation(GLSLShaderType3, "PCFMode");
	}
	// еще одна проверка перед тем как будем использовать шадовмеп
	// если не смогли загрузить шейдеры, то делать с шадовмеп нечего
	if (!Setup.UseGLSL) Setup.ShadowMap = 0;


	// инициализация менеджера частиц (обязательно после загрузки шейдеров)
	vw_InitParticleSystems(Setup.UseGLSL, Setup.VisualEffectsQuality+1.0f);




	for (unsigned int i=0; i<CurrentListCount; i++)
	{
		switch (CurrentList[i].FileType)
		{
			// 2d текстуры
			case 0:
				if (vw_FindTextureByName(CurrentList[i].FileName) == 0)
				{
					// установки параметров
					vw_SetTextureAlpha(CurrentList[i].Red, CurrentList[i].Green, CurrentList[i].Blue);
					vw_SetTextureProp(CurrentList[i].TextFiltr, CurrentList[i].TextWrap,
						CurrentList[i].Alpha, CurrentList[i].AlphaMode, CurrentList[i].MipMap);

					vw_LoadTexture(CurrentList[i].FileName, NULL, CurrentList[i].NeedCompression ? Setup.TexturesCompressionType : 0);
				}
				break;


			// текстуры
			case 1:
				if (vw_FindTextureByName(CurrentList[i].FileName) == 0)
				{
					int H = 0;
					int W = 0;

					// установки параметров
					vw_SetTextureAlpha(CurrentList[i].Red, CurrentList[i].Green, CurrentList[i].Blue);
					vw_SetTextureProp(CurrentList[i].TextFiltr, CurrentList[i].TextWrap,
						CurrentList[i].Alpha, CurrentList[i].AlphaMode, CurrentList[i].MipMap);

					// мы можем принудительно менять размер текстур через настройки, но надо учитывать их размеры
					// базовый размер почти всех текстур моделей - 512х512 пикселей, небольшая часть текстур 256х256 (мины, турели)
					// текстуры всех планет - 1024х512
					// "неформатные" текстуры - track.VW2D и asteroid-01.tga, им вообще не надо менять размеры

					if (Setup.TexturesQuality == 1)
					{
						// только для текстур в папке MODELS (скайбоксы никогда не трогаем)
						if (!strncmp("models/", CurrentList[i].FileName, strlen("models/")) &&
							// не меняем размеры небольших текстур вообще
							strcmp("models/track.vw2d", CurrentList[i].FileName) &&
							strcmp("models/space/asteroid-01.tga", CurrentList[i].FileName) &&
							// не меняем размер или ставим спец размер
							strncmp("models/spacebase/", CurrentList[i].FileName, strlen("models/spacebase/")) &&
							strncmp("models/planet/", CurrentList[i].FileName, strlen("models/planet/")) &&
							strncmp("models/normalmap/", CurrentList[i].FileName, strlen("models/normalmap/")) &&
							// не ставим маленький размер для текстур-подсветки файтеров землян - плохо смотрится
							strcmp("models/earthfighter/sf-illum01.vw2d", CurrentList[i].FileName) &&
							strcmp("models/earthfighter/sf-illum02.vw2d", CurrentList[i].FileName) &&
							strcmp("models/earthfighter/sf-illum03.vw2d", CurrentList[i].FileName) &&
							strcmp("models/earthfighter/sf-illum04.vw2d", CurrentList[i].FileName))
						{
							H = W = 128;
						}
						else
						{
							// для подсветки файтеров землян и частей баз (с решетками на альфа канале) - ставим больше размер
							if (!strncmp("models/spacebase/", CurrentList[i].FileName, strlen("models/spacebase/")) ||
								!strcmp("models/earthfighter/sf-illum01.vw2d", CurrentList[i].FileName) ||
								!strcmp("models/earthfighter/sf-illum02.vw2d", CurrentList[i].FileName) ||
								!strcmp("models/earthfighter/sf-illum03.vw2d", CurrentList[i].FileName) ||
								!strcmp("models/earthfighter/sf-illum04.vw2d", CurrentList[i].FileName))
							{
								H = W = 256;
							}
							// текстуры планет не квадратные, учитываем это
							if (!strncmp("models/planet/", CurrentList[i].FileName, strlen("models/planet/")) &&
								strcmp("models/planet/asteroid.tga", CurrentList[i].FileName))
							{
								W = 512; H = 256;
							}
						}
					}
					if (Setup.TexturesQuality == 2)
					{
						// только для текстур в папке MODELS (скайбоксы никогда не трогаем)
						if (!strncmp("models/", CurrentList[i].FileName, strlen("models/")) &&
							// не меняем размеры небольших текстур вообще
							strcmp("models/track.vw2d", CurrentList[i].FileName) &&
							strcmp("models/space/asteroid-01.tga", CurrentList[i].FileName) &&
							// не меняем размер
							strncmp("models/spacebase/", CurrentList[i].FileName, strlen("models/spacebase/")) &&
							strncmp("models/planet/", CurrentList[i].FileName, strlen("models/planet/")) &&
							strncmp("models/normalmap/", CurrentList[i].FileName, strlen("models/normalmap/")))
						{
							H = W = 256;
						}
					}

					// если это карта нормалей, но у нас не включены шейдеры - пропускаем
					if (!strncmp("models/NORMALMAP", CurrentList[i].FileName, strlen("models/NORMALMAP")) && !Setup.UseGLSL) break;

					vw_LoadTexture(CurrentList[i].FileName, NULL, CurrentList[i].NeedCompression ? Setup.TexturesCompressionType : 0, AUTO_FILE, W, H);
				}
				break;

			// предварит. загрузка моделей
			case 2:
				vw_LoadModel3D(CurrentList[i].FileName, CurrentList[i].TriangleSizeLimit, CurrentList[i].NeedTangentAndBinormal && Setup.UseGLSL);
				break;

			// загрузка sfx
			case 4:
				// если вообще можем играть звуки
				if (Setup.Sound_check)
				{
					// если еще не загрузили этот звук
					if (vw_FindBufferIDByName(CurrentList[i].FileName) == 0)
					{
						// проверяем, вообще есть расширение или нет, плюс, получаем указатель на последнюю точку
						const char *file_ext = strrchr(CurrentList[i].FileName, '.');
						if (file_ext)
						{
							if (!strcasecmp(".wav", file_ext)) vw_CreateSoundBufferFromWAV(CurrentList[i].FileName);
							else
								if (!strcasecmp(".ogg", file_ext)) vw_CreateSoundBufferFromOGG(CurrentList[i].FileName);
						}
					}
				}
				break;


		}


		RealLoadedTextures += CurrentList[i].Value;

		// рисуем текущее состояние загрузки, если не рисуем логотип
		DrawLoading(RealLoadedTextures, AllDrawLoading, &LastDrawTime, LoadImageTexture);

	}






#ifdef multithread
	//ждем завершение звука
	LoadSoundThreadNeedOff = true;
	if (SoundThread != 0) SDL_WaitThread(SoundThread, NULL);
#endif //multithread







	// убираем картинку загрузки
	if (LoadImageTexture != 0)
	{
		vw_ReleaseTexture(LoadImageTexture);
	}


AllDataLoaded:


	// инициализируем шадов меп, делаем это постоянно т.к. у нас разные размеры карт для меню и игры
	if (Setup.ShadowMap > 0)
	{
		int ShadowMapSize = 1024;
		switch(Setup.ShadowMap)
		{
			case 1:
			case 2:
			case 3:
					ShadowMapSize = vw_GetDevCaps()->MaxTextureWidth/4; break;
			case 4:
			case 5:
			case 6:
					ShadowMapSize = vw_GetDevCaps()->MaxTextureWidth/2; break;
			case 7:
			case 8:
			case 9:
					ShadowMapSize = vw_GetDevCaps()->MaxTextureWidth; break;
		}

		switch(LoadType)
		{
			case -1:  // меню (только запустили)
				if (!ShadowMap_Init(ShadowMapSize, ShadowMapSize/2)) Setup.ShadowMap = 0;
				break;
			case 0:   // меню (выходим из игры)
				ShadowMap_Release();
				if (!ShadowMap_Init(ShadowMapSize, ShadowMapSize/2)) Setup.ShadowMap = 0;
				break;

			case 1: // переход на уровни игры
				ShadowMap_Release();
				if (!ShadowMap_Init(ShadowMapSize, ShadowMapSize)) Setup.ShadowMap = 0;
				break;
		}
	}


	// переходим в нужное место...
	switch(LoadType)
	{
		// меню, первая загрузка, самый старт
		case -1:
			InitMenu();
			break;
		// меню, выходим из игры, входим в меню
		case 0:
			InitMenu();
			GameStatus = MISSION; // чтобы не было перехода с основного меню в мисии
			Setup.LoadingHint++;
			if (Setup.LoadingHint >= LoadingHintsCount) Setup.LoadingHint = 0;
			break;

		// уровни игры
		case 1:
			// освобождаем память от того, что загружали
			if (CurrentList != 0){delete [] CurrentList; CurrentList = 0;}
			InitGame();
			StartMusicWithFade(2, 2.0f, 2.0f);
			// приготовиться к действию (речь)
			Audio_PlayVoice(5, 1.0f);
			Setup.LoadingHint++;
			if (Setup.LoadingHint >= LoadingHintsCount) Setup.LoadingHint = 0;
			break;
	}


	// всегда на черном фоне
	vw_SetClearColor(0.0f,0.0f,0.0f,1.0f);
}



