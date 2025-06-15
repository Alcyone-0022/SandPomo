# SandPomo
University Of Ulsan - 2025-1SEM Open Source Software Term Project



# SandPomo: 모래시계 콘셉트의 뽀모도로 타이머

울산대학교 OSS 텀프로젝트로 개발된 직관적 물리형 뽀모도로 타이머입니다. 숫자 대신 LED와 기울기 등 직관적인 상호작용을 통해 시간의 흐름을 체감할 수 있도록 설계했습니다.

## Project Overview

- **프로젝트명**: SandPomo
- **진행기간**: 2025년 5월 ~ 2025년 6월
- **팀 이름**: 샌드뽀모
- **팀원**: 하승갑 성영준 윤다연 최승우

## Background & Motivation
“시간이란 무엇일까?”라는 질문에서 출발하여, 시간의 흐름을 시각적으로 표현할 수 있는 ‘모래시계’ 콘셉트를 채택하였습니다. 남은 시간을 숫자 대신 LED의 '모래' 양으로 보여줌으로써, 사용자는 숫자에 대한 압박 없이 더 몰입감 있는 시간관리 경험을 할 수 있습니다.

## Adaptiveness with references
- **돌려쓰는 타이머** -> 기울기로 시간 관리 및 조작 기능
<image src="https://github.com/user-attachments/assets/1ab3db29-8078-4ea2-98a9-e818c8478393" width="300"/>

- **모래시계** -> 숫자 없이 시간 흐름 파악
<image src="https://github.com/user-attachments/assets/e6d4aae7-0f3a-43da-9162-da8b4c1f6313" width="300"/>

- **신호등** -> LED를 접목 시켜 시간 흐름을 직관적으로 제공
<image src="https://github.com/user-attachments/assets/ac950b10-1121-4f6d-bb40-97c804a011f8" width="300"/>


## Concept & Main Feature

- **하드웨어 구성**
  - ESP32, ARGB LED 스트립, 자이로 센서
  - 18650 배터리 사용
  - 3D 프린터로 출력한 모래시계 모양 외관
  <image src="https://github.com/user-attachments/assets/07393ead-2a27-4a81-944d-3352364a7195" width="300"/>
  


- **세션 표시**
  - 집중 시간: 빨간색 LED
  <image src="https://github.com/user-attachments/assets/eef47e84-1c15-4a09-8660-d892abb6a90c" width="300"/>

  - 휴식 시간: 초록색 LED
  <image src="https://github.com/user-attachments/assets/dee28944-4079-4c49-986c-354d38ffd918" width="300"/>

  - 일시정지: 노란색 LED
  <image src="https://github.com/user-attachments/assets/8db5220b-83df-4c03-a20d-a13290a65e1b" width="300"/>
 
  - 세션 표기: 파란색 LED
  <image src="https://github.com/user-attachments/assets/d3e1aa19-b793-49ac-9acb-3205f07439c8" width="300"/>


  - LED가 아래로 내려가는 방식으로 시간 흐름 표현

- **직관적 제어 방식**
  - 기기를 **뒤집으면** 타이머 시작/재시작
  - **눕히면** 일시정지
  - 버튼 없는 물리적 조작

- **소프트웨어 구성**
  - Python과 Tkinter 기반 GUI
  - USB를 통해 타이머 세션 설정 가능
  <image src="https://github.com/user-attachments/assets/855ebb20-bc13-48af-ac07-4f1a52690095" width="300"/>


## Development process

| 단계 | 기간       | 내용                            |
|------|------------|---------------------------------|
| 1단계 | ~ 5월 중순 | 주제 선정 및 목표 설정          |
| 2단계 | ~ 5월 하순 | 팀의 강점을 살려 HW 기반 기획   |
| 3단계 | 5~6월     | H/W, S/W, 모델링, 테스트 진행    |
| 4단계 | 6월 이후  | 발표자료 및 시연 영상 제작       |

## Used tools

- **디자인**: Blender, Fusion 360, draw.io
- **프로그래밍**: Python, Tkinter
- **하드웨어**: ESP32, ARGB LED, 3D 프린터 등

## Futeure works

- 키링 크기의 초소형 모델 개발
- 조작의 단순화 및 사용성 개선
- 하드웨어 및 소프트웨어 안정성 강화
