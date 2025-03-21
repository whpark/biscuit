﻿module;

//////////////////////////////////////////////////////////////////////
//
// canvas.h:
//
// PWH
// 2017.07.20
// 2021.06.01 from Mocha
// 2024-10-10. biscuit
//
///////////////////////////////////////////////////////////////////////////////

export module biscuit.shape.color_table;
import std;
import biscuit;
import biscuit.shape_basic;

export namespace biscuit::shape {

	constexpr std::array<color_t, 256> const s_tblColor {{
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0),        (uint8_t)(255*0) ),                        // unused
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0),        (uint8_t)(255*0) ),                        // 1
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*1),        (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*1),        (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*1),        (uint8_t)(255*1) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0),        (uint8_t)(255*1) ),        
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0),        (uint8_t)(255*1) ),        
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*1),        (uint8_t)(255*1) ),                        // black or white
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.5),      (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.75),     (uint8_t)(255*0.75),     (uint8_t)(255*0.75) ),        
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0),        (uint8_t)(255*0) ),                        // 10
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0.5),      (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0),        (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0.325),    (uint8_t)(255*0.325) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0),        (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.25),     (uint8_t)(255*0.25) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0),        (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0.15),     (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0),        (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.075),    (uint8_t)(255*0.075) ),        
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0.25),     (uint8_t)(255*0) ),                     // 20
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0.625),    (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0.1625),   (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0.4063),   (uint8_t)(255*0.325) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.125),    (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.3125),   (uint8_t)(255*0.25) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0.075),    (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0.1875),   (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.0375),   (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.0938),   (uint8_t)(255*0.075) ),        
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0.5),      (uint8_t)(255*0) ),                      // 30
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0.75),     (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0.325),    (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0.4875),   (uint8_t)(255*0.325) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.25),     (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.375),    (uint8_t)(255*0.25) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0.15),     (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0.225),    (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.075),    (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.1125),   (uint8_t)(255*0.075) ),        
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0.75),     (uint8_t)(255*0) ),                     // 40
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0.875),    (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0.4875),   (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0.5688),   (uint8_t)(255*0.325) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.375),    (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.4375),   (uint8_t)(255*0.25) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0.225),    (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0.2625),   (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.1125),   (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.1313),   (uint8_t)(255*0.075) ),        
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*1),        (uint8_t)(255*0) ),                        // 50
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*1),        (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0.65),     (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0.65),     (uint8_t)(255*0.325) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.5),      (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.5),      (uint8_t)(255*0.25) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0.3),      (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0.3),      (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.15),     (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.15),     (uint8_t)(255*0.075) ),        
		ColorRGBA( (uint8_t)(255*0.75),     (uint8_t)(255*1),        (uint8_t)(255*0) ),                     // 60
		ColorRGBA( (uint8_t)(255*0.875),    (uint8_t)(255*1),        (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.4875),   (uint8_t)(255*0.65),     (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.5688),   (uint8_t)(255*0.65),     (uint8_t)(255*0.325) ),        
		ColorRGBA( (uint8_t)(255*0.375),    (uint8_t)(255*0.5),      (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.4375),   (uint8_t)(255*0.5),      (uint8_t)(255*0.25) ),        
		ColorRGBA( (uint8_t)(255*0.225),    (uint8_t)(255*0.3),      (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.2625),   (uint8_t)(255*0.3),      (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.1125),   (uint8_t)(255*0.15),     (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.1313),   (uint8_t)(255*0.15),     (uint8_t)(255*0.075) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*1),        (uint8_t)(255*0) ),                      // 70
		ColorRGBA( (uint8_t)(255*0.75),     (uint8_t)(255*1),        (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.325),    (uint8_t)(255*0.65),     (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.4875),   (uint8_t)(255*0.65),     (uint8_t)(255*0.325) ),        
		ColorRGBA( (uint8_t)(255*0.25),     (uint8_t)(255*0.5),      (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.375),    (uint8_t)(255*0.5),      (uint8_t)(255*0.25) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.3),      (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.225),    (uint8_t)(255*0.3),      (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.075),    (uint8_t)(255*0.15),     (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.1125),   (uint8_t)(255*0.15),     (uint8_t)(255*0.075) ),        
		ColorRGBA( (uint8_t)(255*0.25),     (uint8_t)(255*1),        (uint8_t)(255*0) ),                     // 80
		ColorRGBA( (uint8_t)(255*0.625),    (uint8_t)(255*1),        (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.1625),   (uint8_t)(255*0.65),     (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.4063),   (uint8_t)(255*0.65),     (uint8_t)(255*0.325) ),        
		ColorRGBA( (uint8_t)(255*0.125),    (uint8_t)(255*0.5),      (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.3125),   (uint8_t)(255*0.5),      (uint8_t)(255*0.25) ),        
		ColorRGBA( (uint8_t)(255*0.075),    (uint8_t)(255*0.3),      (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.1875),   (uint8_t)(255*0.3),      (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.0375),   (uint8_t)(255*0.15),     (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.0938),   (uint8_t)(255*0.15),     (uint8_t)(255*0.075) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*1),        (uint8_t)(255*0) ),                        // 90
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*1),        (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.65),     (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.325),    (uint8_t)(255*0.65),     (uint8_t)(255*0.325) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.5),      (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.25),     (uint8_t)(255*0.5),      (uint8_t)(255*0.25) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.3),      (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.3),      (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.15),     (uint8_t)(255*0) ),        
		ColorRGBA( (uint8_t)(255*0.075),    (uint8_t)(255*0.15),     (uint8_t)(255*0.075) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*1),        (uint8_t)(255*0.25) ),                     // 100
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*1),        (uint8_t)(255*0.625) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.65),     (uint8_t)(255*0.1625) ),        
		ColorRGBA( (uint8_t)(255*0.325),    (uint8_t)(255*0.65),     (uint8_t)(255*0.4063) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.5),      (uint8_t)(255*0.125) ),        
		ColorRGBA( (uint8_t)(255*0.25),     (uint8_t)(255*0.5),      (uint8_t)(255*0.3125) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.3),      (uint8_t)(255*0.075) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.3),      (uint8_t)(255*0.1875) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.15),     (uint8_t)(255*0.0375) ),        
		ColorRGBA( (uint8_t)(255*0.075),    (uint8_t)(255*0.15),     (uint8_t)(255*0.0938) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*1),        (uint8_t)(255*0.5) ),                      // 110
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*1),        (uint8_t)(255*0.75) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.65),     (uint8_t)(255*0.325) ),        
		ColorRGBA( (uint8_t)(255*0.325),    (uint8_t)(255*0.65),     (uint8_t)(255*0.4875) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.5),      (uint8_t)(255*0.25) ),        
		ColorRGBA( (uint8_t)(255*0.25),     (uint8_t)(255*0.5),      (uint8_t)(255*0.375) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.3),      (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.3),      (uint8_t)(255*0.225) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.15),     (uint8_t)(255*0.075) ),        
		ColorRGBA( (uint8_t)(255*0.075),    (uint8_t)(255*0.15),     (uint8_t)(255*0.1125) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*1),        (uint8_t)(255*0.75) ),                     // 120
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*1),        (uint8_t)(255*0.875) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.65),     (uint8_t)(255*0.4875) ),        
		ColorRGBA( (uint8_t)(255*0.325),    (uint8_t)(255*0.65),     (uint8_t)(255*0.5688) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.5),      (uint8_t)(255*0.375) ),        
		ColorRGBA( (uint8_t)(255*0.25),     (uint8_t)(255*0.5),      (uint8_t)(255*0.4375) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.3),      (uint8_t)(255*0.225) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.3),      (uint8_t)(255*0.2625) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.15),     (uint8_t)(255*0.1125) ),        
		ColorRGBA( (uint8_t)(255*0.075),    (uint8_t)(255*0.15),     (uint8_t)(255*0.1313) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*1),        (uint8_t)(255*1) ),                        // 130
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*1),        (uint8_t)(255*1) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.65),     (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0.325),    (uint8_t)(255*0.65),     (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.5),      (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.25),     (uint8_t)(255*0.5),      (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.3),      (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.3),      (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.15),     (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.075),    (uint8_t)(255*0.15),     (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.75),     (uint8_t)(255*1) ),                     // 140
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.875),    (uint8_t)(255*1) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.4875),   (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0.325),    (uint8_t)(255*0.5688),   (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.375),    (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.25),     (uint8_t)(255*0.4375),   (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.225),    (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.2625),   (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.1125),   (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.075),    (uint8_t)(255*0.1313),   (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.5),      (uint8_t)(255*1) ),                      // 150
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.75),     (uint8_t)(255*1) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.325),    (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0.325),    (uint8_t)(255*0.4875),   (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.25),     (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.25),     (uint8_t)(255*0.375),    (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.15),     (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.225),    (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.075),    (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.075),    (uint8_t)(255*0.1125),   (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.25),     (uint8_t)(255*1) ),                     // 160
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.625),    (uint8_t)(255*1) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.1625),   (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0.325),    (uint8_t)(255*0.4063),   (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.125),    (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.25),     (uint8_t)(255*0.3125),   (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.075),    (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.1875),   (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0.0375),   (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.075),    (uint8_t)(255*0.0938),   (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0),        (uint8_t)(255*1) ),                        // 170
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.5),      (uint8_t)(255*1) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0),        (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0.325),    (uint8_t)(255*0.325),    (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0),        (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.25),     (uint8_t)(255*0.25),     (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0),        (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.15),     (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0),        (uint8_t)(255*0),        (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.075),    (uint8_t)(255*0.075),    (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.25),     (uint8_t)(255*0),        (uint8_t)(255*1) ),                     // 180
		ColorRGBA( (uint8_t)(255*0.625),    (uint8_t)(255*0.5),      (uint8_t)(255*1) ),        
		ColorRGBA( (uint8_t)(255*0.1625),   (uint8_t)(255*0),        (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0.4063),   (uint8_t)(255*0.325),    (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0.125),    (uint8_t)(255*0),        (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.3125),   (uint8_t)(255*0.25),     (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.075),    (uint8_t)(255*0),        (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0.1875),   (uint8_t)(255*0.15),     (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0.0375),   (uint8_t)(255*0),        (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.0938),   (uint8_t)(255*0.075),    (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0),        (uint8_t)(255*1) ),                      // 190
		ColorRGBA( (uint8_t)(255*0.75),     (uint8_t)(255*0.5),      (uint8_t)(255*1) ),        
		ColorRGBA( (uint8_t)(255*0.325),    (uint8_t)(255*0),        (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0.4875),   (uint8_t)(255*0.325),    (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0.25),     (uint8_t)(255*0),        (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.375),    (uint8_t)(255*0.25),     (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0),        (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0.225),    (uint8_t)(255*0.15),     (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0.075),    (uint8_t)(255*0),        (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.1125),   (uint8_t)(255*0.075),    (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.75),     (uint8_t)(255*0),        (uint8_t)(255*1) ),                     // 200
		ColorRGBA( (uint8_t)(255*0.875),    (uint8_t)(255*0.5),      (uint8_t)(255*1) ),        
		ColorRGBA( (uint8_t)(255*0.4875),   (uint8_t)(255*0),        (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0.5688),   (uint8_t)(255*0.325),    (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0.375),    (uint8_t)(255*0),        (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.4375),   (uint8_t)(255*0.25),     (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.225),    (uint8_t)(255*0),        (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0.2625),   (uint8_t)(255*0.15),     (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0.1125),   (uint8_t)(255*0),        (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.1313),   (uint8_t)(255*0.075),    (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0),        (uint8_t)(255*1) ),                        // 210
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0.5),      (uint8_t)(255*1) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0),        (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0.325),    (uint8_t)(255*0.65) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0),        (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.25),     (uint8_t)(255*0.5) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0),        (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0.15),     (uint8_t)(255*0.3) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0),        (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.075),    (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0),        (uint8_t)(255*0.75) ),                     // 220
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0.5),      (uint8_t)(255*0.875) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0),        (uint8_t)(255*0.4875) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0.325),    (uint8_t)(255*0.5688) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0),        (uint8_t)(255*0.375) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.25),     (uint8_t)(255*0.4375) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0),        (uint8_t)(255*0.225) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0.15),     (uint8_t)(255*0.2625) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0),        (uint8_t)(255*0.1125) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.075),    (uint8_t)(255*0.1313) ),        
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0),        (uint8_t)(255*0.5) ),                      // 230
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0.5),      (uint8_t)(255*0.75) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0),        (uint8_t)(255*0.325) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0.325),    (uint8_t)(255*0.4875) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0),        (uint8_t)(255*0.25) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.25),     (uint8_t)(255*0.375) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0),        (uint8_t)(255*0.15) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0.15),     (uint8_t)(255*0.225) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0),        (uint8_t)(255*0.075) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.075),    (uint8_t)(255*0.1125) ),        
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0),        (uint8_t)(255*0.25) ),                     // 240
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*0.5),      (uint8_t)(255*0.625) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0),        (uint8_t)(255*0.1625) ),        
		ColorRGBA( (uint8_t)(255*0.65),     (uint8_t)(255*0.325),    (uint8_t)(255*0.4063) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0),        (uint8_t)(255*0.125) ),        
		ColorRGBA( (uint8_t)(255*0.5),      (uint8_t)(255*0.25),     (uint8_t)(255*0.3125) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0),        (uint8_t)(255*0.075) ),        
		ColorRGBA( (uint8_t)(255*0.3),      (uint8_t)(255*0.15),     (uint8_t)(255*0.1875) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0),        (uint8_t)(255*0.0375) ),        
		ColorRGBA( (uint8_t)(255*0.15),     (uint8_t)(255*0.075),    (uint8_t)(255*0.0938) ),        
		ColorRGBA( (uint8_t)(255*0.33),     (uint8_t)(255*0.33),     (uint8_t)(255*0.33) ),               // 250
		ColorRGBA( (uint8_t)(255*0.464),    (uint8_t)(255*0.464),    (uint8_t)(255*0.464) ),        
		ColorRGBA( (uint8_t)(255*0.598),    (uint8_t)(255*0.598),    (uint8_t)(255*0.598) ),        
		ColorRGBA( (uint8_t)(255*0.732),    (uint8_t)(255*0.732),    (uint8_t)(255*0.732) ),        
		ColorRGBA( (uint8_t)(255*0.866),    (uint8_t)(255*0.866),    (uint8_t)(255*0.866) ),        
		ColorRGBA( (uint8_t)(255*1),        (uint8_t)(255*1),        (uint8_t)(255*1) ),                 // 255
	}};

	uint8_t GetApproxColorIndex(color_t cr) {
		uint32_t dwMinError = (uint32_t)-1;
		uint8_t iCR = 0;
		for (size_t i = 0; i < s_tblColor.size(); i++) {
			auto cr2 = s_tblColor[i];
			uint32_t dwError = Square((int)cr.r-(int)cr2.r) + Square((int)cr.g-(int)cr2.g) + Square((int)cr.b-(int)cr2.b);
			if (!dwError) {
				iCR = (uint8_t)i;
				return iCR;
			}
			if (dwMinError > dwError) {
				dwMinError = dwError;
				iCR = (uint8_t)i;
			}
		}
		return iCR;
	}


}	// export namespace biscuit::shape

