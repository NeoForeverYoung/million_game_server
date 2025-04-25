const fs = require('fs');
const path = require('path');
const iconv = require('iconv-lite');

// 中文注释对照表
const commentMap = {
  // 1_simple_server.js
  '���յ�����': '接收到数据',
  '����,': '回复,',
  '�Ͽ�����': '断开连接',
  
  // 2_walk_server.js & 3_walk_server.js
  '������': '新连接',
  '����λ��': '更新位置',
  '�㲥': '广播',
  
  // 3_chat_server.js
  '�յ�����': '收到消息'
};

// 要处理的文件列表
const files = [
  'Chapter1/1_simple_server.js',
  'Chapter1/2_walk_server.js',
  'Chapter1/3_chat_server.js',
  'Chapter1/3_walk_server.js'
];

// 处理每个文件
files.forEach(filePath => {
  try {
    // 读取文件内容
    const content = fs.readFileSync(filePath, 'utf8');
    
    // 替换所有中文乱码
    let newContent = content;
    for (const [garbled, chinese] of Object.entries(commentMap)) {
      newContent = newContent.replace(new RegExp(garbled, 'g'), chinese);
    }
    
    // 写回文件
    fs.writeFileSync(filePath, newContent, 'utf8');
    console.log(`已修复: ${filePath}`);
  } catch (err) {
    console.error(`处理 ${filePath} 时出错:`, err);
  }
});

console.log('所有文件处理完成!'); 