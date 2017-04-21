/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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
