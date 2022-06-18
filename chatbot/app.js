const express=require('express');
const db=require('./db');
const ubi=require('ubicrypt');
const app=express();

// 미들웨어 등록
app.use(express.urlencoded({extended:false})); // post데이터를 request.body로 받음
app.use(express.json()); // request body를 JSON으로 파싱(분석 결과를 json파일로 저장)

// custom_list 파일: n,e,d값이 차례대로 저장된 파일
// 커스텀 파일 읽고 변수에 저장
const fs = require('fs');
let custom_data = fs.readFileSync('./custom_list.txt','utf8').split('\n');
let custom_n_array=[];
let custom_e_array=[];
custom_data.forEach((item,index)=>{
  if(index < custom_data.length-1){
    item=item.slice(0,item.length-1);
    custom_n_array[index]=parseInt(item.split(' ')[0],10); // b array
    custom_e_array[index]=parseInt(item.split(' ')[1],10); // u array
  }
})
let custom_size=custom_e_array.length;


// 봇과 연결된 라우트 (사용자가 챗봇에 메시지를 전송하면 서버의 'func' path로 요청하는 것과 같음)
app.post('/func', (req, res) => {
  const question = req.body.userRequest.utterance;
  const userId=req.body.userRequest.user.id;
  const goMain = '처음으로';
  let data='';

  // db.json에서 Id:username인 유저 객체 찾아서 리턴
  let user = db.get('users').find({
       Id: userId
  }).value();

  // 등록된 회원인 경우
  if(user){
    // 원상복귀
    if((question === '처음으로') || (question === '처음')){
      // 응답 데이터 수정
      data={
        'version': '2.0',
        'template': {
        'outputs': [{
          'simpleText': {
            'text': `1. 암호를 만들어 드릴까요?\n2. 암호를 해독해 드릴까요?`
            }
          }]
        }
      }
      // 일반 & 분할 모드
      if(user.mode<2){
        // status 및 current_ubcode 수정 <원상 복귀>
        db.get('users').find({Id: userId}).assign({
          status:0,
          current_ucode:0,
          current_bcode:0
        }).write();
      }
      // 커스텀 모드
      else{
        // status 및 current_ubcode 수정 <원상 복귀>
        db.get('users').find({Id: userId}).assign({
          status:20,
          current_ucode:0,
          current_bcode:0
        }).write();
      }
    }

    // 회원 탈퇴
    else if(question === '/회원탈퇴'){
      // 응답 데이터 수정
      data={
        'version': '2.0',
        'template': {
        'outputs': [{
          'simpleText': {
            'text': `탈퇴되었습니다. 다시 등록하고 싶으시다면 '우비'를 불러 주세요!!`
            }
          }]
        }
      }
      // db에서 회원 삭제
      db.get('users').remove({Id:userId}).write();
    }

    // 현재 모드 보기
    else if((question === '/상황') || (question === '/상태')){
      let status='';

      if(user.mode === 0)
        status = '일반 모드';
      else if(user.mode === 1)
        status = '분할 모드';
      else
        status = '커스텀 모드';

      // 응답 데이터 수정
      data={
        'version': '2.0',
        'template': {
        'outputs': [{
          'simpleText': {
            'text': `현재 ${status}입니다!`
            }
          }],
          'quickReplies': [{
            'label': goMain,
            'action': 'message',
            'messageText': goMain
          }]
        }
      }
    }

    // 일반 모드 세팅
    else if((question === '/일반 모드')||(question === '/일반모드')||(question === '/일반')){
      if(user.mode === 0){
        // 응답 데이터 수정
        data={
          'version': '2.0',
          'template': {
          'outputs': [{
            'simpleText': {
              'text': `이미 일반 모드입니다!`
              }
            }],
            'quickReplies': [{
              'label': goMain,
              'action': 'message',
              'messageText': goMain
            }]
          }
        }
      }
      else{
        // 응답 데이터 수정
        data={
          'version': '2.0',
          'template': {
          'outputs': [{
            'simpleText': {
              'text': `일반 모드입니다. 간편하지만 보안 상 취약할 수 있습니다!`
              }
            }],
            'quickReplies': [{
              'label': goMain,
              'action': 'message',
              'messageText': goMain
            }]
          }
        }
        // mode,status 수정
        db.get('users').find({Id: userId}).assign({
          mode:0,
          status:0
        }).write();
      }
    }

    // 분할 모드 세팅
    else if((question === '/분할 모드')||(question === '/분할모드')||(question === '/분할')){
      if(user.mode === 1){
        // 응답 데이터 수정
        data={
          'version': '2.0',
          'template': {
          'outputs': [{
            'simpleText': {
              'text': `이미 분할 모드입니다!`
              }
            }],
            'quickReplies': [{
              'label': goMain,
              'action': 'message',
              'messageText': goMain
            }]
          }
        }
      }
      else{
        // 응답 데이터 수정
        data={
          'version': '2.0',
          'template': {
          'outputs': [{
            'simpleText': {
              'text': `분할 모드입니다. 암호화 시 암호문과 우비 코드를 나눠서 제공합니다!`
              }
            }],
            'quickReplies': [{
              'label': goMain,
              'action': 'message',
              'messageText': goMain
            }]
          }
        }
        // mode,status 수정
        db.get('users').find({Id: userId}).assign({
          mode:1,
          status:0
        }).write();
      }
    }

    // 커스텀 모드 세팅
    else if((question === '/커스텀 모드')||(question === '/커스텀모드')||(question === '/커스텀')){
      if(user.mode === 2){
        // 응답 데이터 수정
        data={
          'version': '2.0',
          'template': {
          'outputs': [{
            'simpleText': {
              'text': `이미 커스텀 모드입니다!`
              }
            }],
            'quickReplies': [{
              'label': goMain,
              'action': 'message',
              'messageText': goMain
            }]
          }
        }
      }
      else{
        // 응답 데이터 수정
        data={
          'version': '2.0',
          'template': {
          'outputs': [{
            'simpleText': {
              'text': `커스텀 모드입니다.\n사용자 맞춤 코드(커스텀 코드)로 암호화와 복호화가 가능합니다!`
              }
            }],
            'quickReplies': [{
              'label': goMain,
              'action': 'message',
              'messageText': goMain
            }]
          }
        }
        // mode,status 수정
        db.get('users').find({Id: userId}).assign({
          mode:2,
          status:20
        }).write();
      }
    }

    // 일반모드 & 분할모드
    else if(user.mode <2){
      // 입력 & 분할-선택 상태
      if(user.status === 0){
        // '우비야' 발화
        if((question === '우비야') ||  (question === '우비') || (question === '야')){
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `1. 암호를 만들어 드릴까요?\n2. 암호를 해독해 드릴까요?`
                }
              }]
            }
          }
        }
        else if((question === '만들어줘') ||  (question === '만들어 줘') || (question === '1')){
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': '암호화할 글을 작성해주세요!'
                }
              }]
            }
          }
          // status 수정
          db.get('users').find({Id: userId}).assign({
            status: 1
          }).write();
        }
        else if((question === '해독해줘') || (question === '해독해 줘') || (question === '2')){
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `우코드와 비코드를 작성해주세요. \n ex) 17763 13174`
                }
              }]
            }
          }
          // status 수정
          db.get('users').find({Id: userId}).assign({
            status: 2
          }).write();
        }
        else if((question === '빠른 해독') || (question === '빠른해독') || (question === 'ㄱㄱ')){
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `아래 양식을 지켜서 우비코드와 암호문을 전송해주세요!!\n\nex)\n우코드는 12345입니다.\n비코드는 6789입니다.\n\n암호문은 다음과 같습니다.\n\nUBUUUIUBBUI`
                }
              }]
            }
          }
          // status 수정
          db.get('users').find({Id: userId}).assign({
            status: 4
          }).write();
        }
        else if((question === '글자수') || (question === '글자  수') || (question === 'ㄱㅈㅅ')){
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `글을 전송하시면 글자 수를 세 드릴게요!!`
                }
              }]
            }
          }
          // status 수정
          db.get('users').find({Id: userId}).assign({
            status: 5
          }).write();
        }
      else{
        // 응답 데이터 수정
        data={
          'version': '2.0',
          'template': {
          'outputs': [{
            'simpleText': {
              'text': `무엇을 원하는지 잘 모르겠어요.`
              }
            }]
          }
        }
       }
      }

      // (이전 응답 = '암호화할 글을 작성해주세요!')
      else if(user.status === 1){
        if(question.length>100){
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `죄송합니다. 서버 안정화를 위해 100자 초과의 글은 암호화가 불가능합니다. 처음부터 다시 진행해주세요.`
                }
              }],
              'quickReplies': [{
                'label': goMain,
                'action': 'message',
                'messageText': goMain
              }]
            }
          }
          // 원상 복귀
          db.get('users').find({Id: userId}).assign({
            status: 0
          }).write();
        }
        else{
          let enc=ubi.Encryption(question);
          let string=``;

          // 일반 모드 -> 우비 코드와 암호문을 같이 보냄
          if(user.mode === 0){
             string=`우코드는 ${enc.u_code}입니다.\n비코드는 ${enc.b_code}입니다.\n\n암호문은 다음과 같습니다.\n\n`
             string+=enc.string;

             // 400자 초과 시 basicCard에 입력 불가
             if(string.length > 400){
               data={
                 'version': '2.0',
                 'template': {
                 'outputs': [{
                   'simpleText': {
                     'text': `${string}`
                     }
                   }]
                 }
               }
             }
             else{
               data={
                 'version': '2.0',
                 'template': {
                 'outputs': [{
                   basicCard:{
                     description: `${string}`,
                     buttons:[{
                       action:"share",
                       label:"공유하기",
                     }]
                   }
                 }],
                 'quickReplies': [{
                   'label': goMain,
                   'action': 'message',
                   'messageText': goMain
                 }]
                 }
               }
             }
             // 원상 복귀
             db.get('users').find({Id: userId}).assign({
               status: 0
             }).write();
           }

          // 분할 모드 -> '코드' 명령이 오면 응답해야 됨
          else{
            string=enc.string;
            // 400자 초과 시 basicCard에 입력 불가
            if(string.length > 400){
              data={
                'version': '2.0',
                'template': {
                'outputs': [{
                  'simpleText': {
                    'text': `${string}`
                    }
                  }]
                }
              }
            }
            else{
              data={
                'version': '2.0',
                'template': {
                'outputs': [{
                  basicCard:{
                    description: `${string}`,
                    buttons:[{
                      action:"share",
                      label:"공유하기",
                    }]
                  }
                }]
                }
              }
            }
            // 다음 입력 기대 ('코드')
            db.get('users').find({Id: userId}).assign({
              status: 11,
              current_ucode:enc.u_code,
              current_bcode:enc.b_code
            }).write();
          }
        }
      }

      // (이전 응답 = 'UBBIBBI' <분할 모드-암호문만 간 상태>)
      else if(user.status === 11){
        if(question==="코드"){
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              basicCard:{
                description: `${user.current_ucode} ${user.current_bcode}`,
                buttons:[{
                  action:"share",
                  label:"공유하기",
                }]
              }
            }],
            'quickReplies': [{
              'label': goMain,
              'action': 'message',
              'messageText': goMain
            }]
            }
          }
          // 원상 복귀
          db.get('users').find({Id: userId}).assign({
            status: 0,
            current_ucode:0,
            current_bcode:0
          }).write();
        }
        else{
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `'코드'라고 입력해주세요!!`
                }
              }]
            }
          }
        }
      }

      // (이전 응답 = `우코드와 비코드를 작성해주세요. \n ex) 17763 13174`)
      else if(user.status === 2){
        try{
          // 문자열 분리
          array=question.split(' ');
          // current_ubcode 수정
          db.get('users').find({Id: userId}).assign({
            current_ucode:array[0],
            current_bcode:array[1]
          }).write();

          // 분리가 성공적으로 된 경우
          if((parseInt(array[0])>0) && (parseInt(array[1]) > 0)){
            data={
              'version': '2.0',
              'template': {
              'outputs': [{
                'simpleText': {
                  'text': '해독할 암호문을 작성해주세요!'
                  }
                }]
              }
           }
           // status 수정
           db.get('users').find({Id: userId}).assign({
             status:3
           }).write();
         }
       // 분리 실패한 경우
        else{
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': '잘못된 입력 형식입니다. 우비코드를 올바르게 입력해주세요!'
                }
              }]
            }
         }
       }
      }
      // 이상한 입력해서 아예 오류 걸림
      catch(e){
        data={
          'version': '2.0',
          'template': {
          'outputs': [{
            'simpleText': {
              'text': '잘못된 입력 형식입니다. 우비코드를 올바르게 입력해주세요!'
              }
            }]
          }
        }
      }
     }

      // (이전 응답 = '해독할 암호문을 작성해주세요!')
      else if(user.status === 3){
        if(question.length>3000){
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `죄송합니다. 서버 안정화를 위해 3000자 초과의 글은 복호화가 불가능합니다. 처음부터 다시 진행해주세요.`
                }
              }],
              'quickReplies': [{
                'label': goMain,
                'action': 'message',
                'messageText': goMain
              }]
            }
          }
        }
        else{
          let dec=ubi.Decryption(question,parseInt(user.current_ucode),parseInt(user.current_bcode));

          if((dec.string !== undefined) && (dec.string !== '')){
            let string=`복호문은 다음과 같습니다.\n\n`
            string+=dec.string;
            data={
              'version': '2.0',
              'template': {
              'outputs': [{
                'simpleText': {
                  'text': `${string}`
                  }
                }],
                'quickReplies': [{
                  'label': goMain,
                  'action': 'message',
                  'messageText': goMain
                }]
              }
            }
          }

          else{
            data={
              'version': '2.0',
              'template': {
              'outputs': [{
                'simpleText': {
                  'text': '잘못된 우비코드 혹은 암호문입니다. 처음부터 다시 진행해주세요.'
                  }
                }],
                'quickReplies': [{
                  'label': goMain,
                  'action': 'message',
                  'messageText': goMain
                }]
              }
           }
          }
        }

        // status 및 current_ubcode 수정 <원상 복귀>
        db.get('users').find({Id: userId}).assign({
          status:0,
          current_ucode:0,
          current_bcode:0
        }).write();
      }

      // (이전 응답 = '아래 양식을 지켜서 우비코드와 암호문을 전송해주세요!!')
      else if(user.status === 4){
        if(question.length>3000){
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `죄송합니다. 서버 안정화를 위해 3000자 초과의 글은 복호화가 불가능합니다. 처음부터 다시 진행해주세요.`
                }
              }],
              'quickReplies': [{
                'label': goMain,
                'action': 'message',
                'messageText': goMain
              }]
            }
          }
        }
      // 3000자 이하
      else{
        try{
          let split_question=question.split("\n");
          let str='';
          let ub=[];
          split_question.forEach((item,index)=>{
            if(index<2){
              str='';
              for(i=0; i<item.length;i++){
                if(parseInt(item[i])>=0 && parseInt(item[i])<=9){
                  str+=item[i];
                }
              }
              ub[index]=parseInt(str);
            }
          })
          // 문자열 분리 만족
          if((ub[0]>0) && (ub[1]>0) && ((split_question[5][0]==="U") || (split_question[5][0]==="B") || ((split_question[5][0]==="I")))){
            let dec=ubi.Decryption(split_question[5],ub[0],ub[1]);

            // ubicrypt 정상 작동
            if((dec.string !== undefined) && (dec.string !== '')){
              let string=`복호문은 다음과 같습니다.\n\n`
              string+=dec.string;
              data={
                'version': '2.0',
                'template': {
                'outputs': [{
                  'simpleText': {
                    'text': `${string}`
                    }
                  }],
                  'quickReplies': [{
                    'label': goMain,
                    'action': 'message',
                    'messageText': goMain
                  }]
                }
              }
            }
            // ubicrypt 비정상 작동
            else{
              data={
                'version': '2.0',
                'template': {
                'outputs': [{
                  'simpleText': {
                    'text': '잘못된 우비코드 혹은 암호문입니다. 처음부터 다시 진행해주세요.'
                    }
                  }],
                  'quickReplies': [{
                    'label': goMain,
                    'action': 'message',
                    'messageText': goMain
                  }]
                }
             }
            }
          }
        // 문자열 분리 불만족
        else{
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': '잘못된 우비코드 혹은 암호문입니다. 처음부터 다시 진행해주세요.'
                }
              }],
              'quickReplies': [{
                'label': goMain,
                'action': 'message',
                'messageText': goMain
              }]
            }
         }
        }
        }
        // 이상한 입력해서 아예 오류 걸림
        catch(e){
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': '잘못된 우비코드 혹은 암호문입니다. 처음부터 다시 진행해주세요.'
                }
              }],
              'quickReplies': [{
                'label': goMain,
                'action': 'message',
                'messageText': goMain
              }]
            }
         }
        }
      }
      // status 수정 <원상 복귀>
      db.get('users').find({Id: userId}).assign({
        status:0
      }).write();
    }

    // (이전 응답 = '글자 수를 세 드릴게요!!') -- 모드 상관없이 작동함
    else if(user.status === 5){
      data={
        'version': '2.0',
        'template': {
        'outputs': [{
          'simpleText': {
            'text': `공백을 포함한 글자 수는 ${question.length}자입니다.\n\n공백을 제외한 글자 수는 ${question.replace(/(\s*)/g, "").length}자입니다.`
            }
          }],
          'quickReplies': [{
            'label': goMain,
            'action': 'message',
            'messageText': goMain
          }]
        }
     }
     // status 수정 <원상 복귀>
     db.get('users').find({Id: userId}).assign({
       status:0
     }).write();
    }
    }
    // 일반 & 분할 모드 소스의 끝

    // 커스텀 모드
    else{
      // 커스텀-선택 상태
      if(user.status === 20){
        // '우비야' 발화
        if((question === '우비야') ||  (question === '우비') || (question === '야')){
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `1. 암호를 만들어 드릴까요?\n2. 암호를 해독해 드릴까요?`
                }
              }]
            }
          }
        }
        else if((question === '만들어줘') ||  (question === '만들어 줘') || (question === '1')){
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': '수 (1~9999) 중 사용할 커스텀 코드를 알려 주세요!\n\n해당 코드로 암호를 만들어 드릴게요!'
                }
              }]
            }
          }
          // status 수정
          db.get('users').find({Id: userId}).assign({
            status: 21
          }).write();
        }
        else if((question === '해독해줘') || (question === '해독해 줘') || (question === '2')){
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': '암호화했을 때 사용한 커스텀 코드를 알려 주세요!\n\n해당 코드로 암호를 해독해 드릴게요!'
                }
              }]
            }
          }
          // status 수정
          db.get('users').find({Id: userId}).assign({
            status: 23
          }).write();
        }
        else if((question === '글자수') || (question === '글자  수') || (question === 'ㄱㅈㅅ')){
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `글을 전송하시면 글자 수를 세 드릴게요!!`
                }
              }]
            }
          }
          // status 수정
          db.get('users').find({Id: userId}).assign({
            status: 5
          }).write();
        }
        else{
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `무엇을 원하는지 잘 모르겠어요.`
                }
              }]
            }
          }
        }
      }

      // (이전 응답 = '암호화-커스텀 코드를 알려 주세요!')
      else if(user.status === 21){
        // 커스텀 코드 조건 만족
        if((parseInt(question)>0)&&(parseInt(question)<10000)){
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `암호화할 글을 작성해주세요!!`
                }
              }]
            }
          }
          // status 수정
          db.get('users').find({Id: userId}).assign({
            status: 22,
            current_ucode:custom_e_array[parseInt(question)%custom_size],
            current_bcode:custom_n_array[parseInt(question)%custom_size]
          }).write();
        }
        else{
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `커스텀 코드가 올바르지 않습니다!! 다시 입력해주세요! (범위:1~9999)`
                }
              }]
            }
          }
        }
      }

      // (이전 응답 = '암호화할 글을 작성해주세요!!') <커스텀 코드 입력함>
      else if(user.status === 22){
        if(question.length>100){
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `죄송합니다. 서버 안정화를 위해 100자 초과의 글은 암호화가 불가능합니다. 처음부터 다시 진행해주세요.`
                }
              }],
              'quickReplies': [{
                'label': goMain,
                'action': 'message',
                'messageText': goMain
              }]
            }
          }
        }
        else{
          let enc=ubi.Encryption(question,user.current_ucode,user.current_bcode);
          let string=enc.string;
          // 400자 초과 시 basicCard에 입력 불가
          if(string.length > 400){
            data={
              'version': '2.0',
              'template': {
              'outputs': [{
                'simpleText': {
                  'text': `${string}`
                  }
                }]
              }
            }
          }
          else{
            data={
              'version': '2.0',
              'template': {
              'outputs': [{
                basicCard:{
                  description: `${string}`,
                  buttons:[{
                    action:"share",
                    label:"공유하기",
                  }]
                }
              }],
              'quickReplies': [{
                'label': goMain,
                'action': 'message',
                'messageText': goMain
              }]
              }
            }
          }
        }
        // 원상 복귀
        db.get('users').find({Id: userId}).assign({
          status: 20,
          current_ucode:0,
          current_bcode:0
        }).write();
      }

      // (이전 응답 = '복호화-커스텀 코드를 알려 주세요!')
      else if(user.status === 23){
        // 커스텀 코드 조건 만족
        if((parseInt(question)>0)&&(parseInt(question)<10000)){
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `복호화할 글을 작성해주세요!!`
                }
              }]
            }
          }
          // status 수정
          db.get('users').find({Id: userId}).assign({
            status: 24,
            current_ucode:custom_e_array[parseInt(question)%custom_size],
            current_bcode:custom_n_array[parseInt(question)%custom_size]
          }).write();
        }
        else{
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `커스텀 코드가 올바르지 않습니다!! 다시 입력해주세요! (범위:1~9999)`
                }
              }]
            }
          }
        }
      }

      // (이전 응답 = '복호화할 글을 작성해주세요!!') <커스텀 코드 입력함>
      else if(user.status === 24){
        if(question.length>3000){
          // 응답 데이터 수정
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `죄송합니다. 서버 안정화를 위해 3000자 초과의 글은 복호화가 불가능합니다. 처음부터 다시 진행해주세요.`
                }
              }],
              'quickReplies': [{
                'label': goMain,
                'action': 'message',
                'messageText': goMain
              }]
            }
          }
        }
        else{
          let dec=ubi.Decryption(question,parseInt(user.current_ucode),parseInt(user.current_bcode));

          if((dec.string !== undefined) && (dec.string !== '')){
            let string=`복호문은 다음과 같습니다.\n\n`
            string+=dec.string;
            data={
              'version': '2.0',
              'template': {
              'outputs': [{
                'simpleText': {
                  'text': `${string}`
                  }
                }],
                'quickReplies': [{
                  'label': goMain,
                  'action': 'message',
                  'messageText': goMain
                }]
              }
            }
          }

          else{
            data={
              'version': '2.0',
              'template': {
              'outputs': [{
                'simpleText': {
                  'text': '잘못된 커스텀 코드 혹은 암호문입니다. 처음부터 다시 진행해주세요.'
                  }
                }],
                'quickReplies': [{
                  'label': goMain,
                  'action': 'message',
                  'messageText': goMain
                }]
              }
           }
          }
        }

        // status 및 current_ubcode 수정 <원상 복귀>
        db.get('users').find({Id: userId}).assign({
          status:20,
          current_ucode:0,
          current_bcode:0
        }).write();
        }

        // (이전 응답 = '글자 수를 세 드릴게요!!') -- 모드 상관없이 동작함
        else if(user.status === 5){
          data={
            'version': '2.0',
            'template': {
            'outputs': [{
              'simpleText': {
                'text': `공백을 포함한 글자 수는 ${question.length}자입니다.\n\n공백을 제외한 글자 수는 ${question.replace(/(\s*)/g, "").length}자입니다.`
                }
              }],
              'quickReplies': [{
                'label': goMain,
                'action': 'message',
                'messageText': goMain
              }]
            }
         }
         // status 수정 <원상 복귀>
         db.get('users').find({Id: userId}).assign({
           status:20
         }).write();
        }
      }
      // 커스텀 모드 소스의 끝
  }

  // 등록되지 않은 회원인 경우
  else{
    // '우비야' 발화
    if((question === '우비야') ||  (question === '우비') || (question === '야')){
      db.get('users').push({
           Id: userId,
           status: 0,
           current_ucode:0,
           current_bcode:0,
           mode:0
     }).write();

      data={
        'version': '2.0',
        'template': {
        'outputs': [{
          'simpleText': {
            'text': `축하드립니다!! 사용자 등록이 완료되었습니다.\n\n이제 우비를 부르지 않고 암호를 만들 수 있습니다.\n\n1. 암호를 만들어 드릴까요?\n2. 암호를 해독해 드릴까요?`
            }
          }]
        }
      }
    }

    else{
      data={
        'version': '2.0',
        'template': {
        'outputs': [{
          'simpleText': {
            'text': `등록되지 않은 사용자입니다. '우비'를 불러 사용자 등록을 해주세요!! \n\nex) '우비야' 전송`
            }
          }]
        }
      }
    }
  }

  // data(응답 내용이 담긴 json객체)를 클라이언트에게 보냄
  res.json(data);
})

// 3000번 포트로 동작
app.listen(3000,()=>{
  console.log('server on');
});
