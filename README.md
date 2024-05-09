![2D Simple Smoothing1](https://github.com/frogio/MeshSmoothing/assets/12217092/621b7e8e-08e8-487b-a37f-feeb85cc1631)# Mesh Smoothing
Mesh Smoothing 구현

## 작성 환경/언어
C++, Visual Studio 2019

## 참고 문헌
강남대학교 김종현 교수님 강의 내용

### 프로그램 개요
Simple Smoothing, Laplacian Smoothing, Taubin Smoothing 구현

### Simple 2D, 3D Smoothing
단순 이웃버텍스와 버텍스 자기 자신간의 위치 평균을 자신의 새로운 위치로 삼은 알고리즘 

![2D Simple Smoothing1](https://github.com/frogio/MeshSmoothing/assets/12217092/1e7a7fef-543e-4384-a905-20606b108b53)
Smoothing 수행 이전

![2D Simple Smoothing2](https://github.com/frogio/MeshSmoothing/assets/12217092/85e63878-6262-4c5e-be09-6fb51ba93099)
Smoothing 수행 이후

![Simple Smoothing](https://github.com/frogio/MeshSmoothing/assets/12217092/e33c664f-a236-402d-8b24-1de83c05005e)
3D Smoothing 이후

### Laplacian Smoothing
수축 가중치 하나만을 이용하여 스무딩 수행
버텍스 자기 자신과 이웃 버텍스의 평균간 미분후 가중치를 곱함

![LaplacianSmoothing](https://github.com/frogio/MeshSmoothing/assets/12217092/61f8554c-7560-4a29-a07b-0aed3c8b80f1)
Laplacian Smoothing 이후

### Taubin Smoothing
수축 가중치와 확장 가중치 두개를 이용하여 스무딩 수행

![TaubinSmoothing](https://github.com/frogio/MeshSmoothing/assets/12217092/d655e765-a9e3-455f-acfb-1873e50ee1d9)
Taubin Smoothing 이후
