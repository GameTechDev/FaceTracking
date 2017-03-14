//--------------------------------------------------------------------------------------
// Copyright 2016 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
//--------------------------------------------------------------------------------------
#pragma once

/// Configuration for the sample

/// Landmark width
#define LANDMARK_WIDTH 3
/// Ratio = camera-overlay-width / back-buffer-width
#define CAMERA_OVERLAY_RATIO (1.0f / 3.0f)
/// Color of confident landmarks
#define CONFIDENT_COLOR D3DCOLOR_RGBA(0, 255, 0, 255)
/// Color of unconfident landmarks
#define UNCONFIDENT_COLOR D3DCOLOR_RGBA(255, 0, 0, 255)
/// Font size for intro. text
//#define INTRO_TEXT_FONT_SIZE 20
#define INTRO_TEXT_FONT_SIZE 30
/// Line height for intro. text
#define INTRO_TEXT_LINE_HEIGHT (INTRO_TEXT_FONT_SIZE * 1.2f)
/// Horizontal margin for intro. text
#define INTRO_TEXT_MARGIN_X 10
/// Vertical margin for intro. text
#define INTRO_TEXT_MARGIN_Y 10
/// Foreground color for intro. text
#define INTRO_TEXT_FORGROUND_COLOR D3DCOLOR_RGBA(220, 220, 220, 255)
/// Shadow color for intro. text
#define INTRO_TEXT_SHADOE_COLOR D3DCOLOR_RGBA(10, 10, 10, 255)
/// Background color for intro. text
#define INTRO_TEXT_BG_COLOR D3DCOLOR_RGBA(80, 80, 80, 128)
/// Width of the progress bar 
#define PROGRESS_BAR_WIDTH 200.0f
/// Height of the progress bar 
#define PROGRESS_BAR_HEIGHT 16.0f
/// Horizontal margin of progress bar
#define PROGRESS_BAR_MARGIN_X 8.0f
/// Vertical margin of progress bar
#define PROGRESS_BAR_MARGIN_Y 8.0f
/// Background color of progress bar
#define PROGRESS_BAR_BG_COLOR D3DCOLOR_RGBA(128, 128, 128, 128)
/// Foreground color of progress bar
#define PROGRESS_BAR_FG_COLOR D3DCOLOR_RGBA(255, 255, 0, 255)

/// If using Chinese language
#define TEST_CHINESE_LANGUAGE 0
