# ChatBot UBI
2021년 8월 Node.js 사이드 프로젝트 (개인)

## 💡 Description
![UBI](https://user-images.githubusercontent.com/52392658/174422766-5777b24a-be9d-4427-9e0e-b7386e6268e9.png)

카카오톡 챗봇을 이용해서 우비 암호화를 진행할 수 있습니다. [[View](https://shacoding.com/2021/12/20/%ec%9a%b0%eb%a6%ac%eb%a7%8c%ec%9d%98-%eb%b9%84%eb%b0%80%ec%bd%94%eb%93%9c-ubi-%ec%95%94%ed%98%b8%ed%99%94/)]

**우비 암호화**는 RSA기반 암호화에 UBI(우비) 컨셉을 결합해서 U,B,I 조합의 암호를 만드는 것입니다. [[GitHub](https://github.com/SkiddieAhn/Project-UBI-Encryption)]


## ⚙ Main Function
**암호화와 복호화 (일반 모드)**

> 몇 번의 채팅만으로 편리하게 암호를 만들고 해독할 수 있습니다. <br>
> **만들어줘:** 암호 생성, **해독해줘:** 암호 해독
 
| 일반 모드                                                                                             |
|----------------------------------------------------------------------------------------------------------------------|
|![일반모드](https://user-images.githubusercontent.com/52392658/174423746-32935781-b166-42e2-8944-6be19e7dd7e0.gif) | 


**암호화와 복호화 (분할 모드)**

> 암호화 시 우비 코드와 암호문을 따로 따로 제공합니다. <br>
> **'코드'** 명령어를 이용해서 우비 코드를 획득할 수 있습니다.
 
| 분할 모드                                                                                             |
|----------------------------------------------------------------------------------------------------------------------|
|![분할모드](https://user-images.githubusercontent.com/52392658/174423744-1c244c26-30c2-42ac-b1c5-22327fc063be.gif)|

**암호화와 복호화 (커스텀 모드)**

> 사용자가 지정한 코드(커스텀 코드)로 빠르게 암호를 만들고 해독할 수 있습니다.<br>
> **커스텀 코드**는 1에서 9999까지 사용할 수 있습니다.
 
| 커스텀 모드                                                                                            |
|----------------------------------------------------------------------------------------------------------------------|
|![커스텀모드](https://user-images.githubusercontent.com/52392658/174423747-f3244cf8-5a6e-4d69-9222-0f20fd35edd5.gif) |

## 🛠 Tech Stack

### 1. Server
<strong>웹 앱:</strong> ExpressJS Framework<br>
<strong>웹 서버 컴퓨터:</strong> Amazon EC2 (Free Tier)<br>
<strong>프로세스 매니저:</strong> PM2<br>
<strong>데이터베이스:</strong> lowdb 3.0<br>
<strong>챗봇 API:</strong> Kakao I Open Builder [[View](https://i.kakao.com/)]<br>

### 2. Security
<strong>패키지:</strong> ubicrypt [[GitHub](https://github.com/SkiddieAhn/Project-UBI-Encryption)] [[npm](https://npm.io/package/ubicrypt)]<br>
<strong>암호화:</strong> RSA-17 (직접 구현)<br>
<strong>문자 변환:</strong> Huffman Coding<br>
<strong>한영 변환:</strong> Inko.js [[View](https://github.com/738/inko)]<br>
<strong>참고 도서:</strong> Discrete Mathematics and Its Applications (7th Edition)<br>
