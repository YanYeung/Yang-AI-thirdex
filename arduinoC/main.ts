//% color="#993300" iconWidth=50 iconHeight=40
namespace EdgeCoreAi {



    //% block="中文[STR]的字符数" blockType="reporter"
    //% STR.shadow="string" STR.defl="你好"
    export function countUtf8Characters(parameter: any, block: any) {
        let str = parameter.STR.code;
        Generator.addInclude('count_utf8_include', '#include "countUtf8Characters.h"');
        Generator.addCode([`countUtf8Characters(${str})`, Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="中文[STR]的第[INDEX]个字符" blockType="reporter"
    //% STR.shadow="string" STR.defl="你好"
    //% INDEX.shadow="number" INDEX.defl=1
    export function getUtf8Char(parameter: any, block: any) {
        let str = parameter.STR.code;
        let index = parameter.INDEX.code;
        Generator.addInclude('count_utf8_include', '#include "countUtf8Characters.h"');
        Generator.addCode([`getUtf8CharPointer(${str}, ${index})`, Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="中文[STR]的[CHAR]的索引" blockType="reporter"
    //% STR.shadow="string" STR.defl="你好"
    //% CHAR.shadow="string" CHAR.defl="好"
    export function getUtf8CharIndex(parameter: any, block: any) {
        let str = parameter.STR.code;
        let char = parameter.CHAR.code;
        Generator.addInclude('count_utf8_include', '#include "countUtf8Characters.h"');
        Generator.addCode([`getUtf8CharIndex(${str}, ${char})`, Generator.ORDER_UNARY_POSTFIX]);
    }
    
    
    //% block="初始化本地分类器 服务器IP[IP] 端口[PORT]" blockType="command"
    //% IP.shadow="string" IP.defl="192.168.1.100"
    //% PORT.shadow="number" PORT.defl=8000
    export function initClassifier(parameter: any, block: any) {
        let ip = parameter.IP.code;
        let port = parameter.PORT.code;
        Generator.addInclude('local_classifier_include', '#include <LocalClassifier.h>');
        Generator.addObject('local_classifier_obj', 'LocalClassifier', 'classifier;');
        Generator.addSetup('local_classifier_init', `classifier.setBaseUrl(${ip},${port});`);
    }

    //% block="文本分类[TEXT]结果" blockType="reporter"
    //% TEXT.shadow="string" TEXT.defl="hello"
    export function classifyText(parameter: any, block: any) {
        let text = parameter.TEXT.code;
        Generator.addInclude('local_classifier_include', '#include <LocalClassifier.h>');
        Generator.addObject('local_classifier_obj', 'LocalClassifier', 'classifier;');
        Generator.addCode([`classifier.classifyText(${text})`, Generator.ORDER_UNARY_POSTFIX]);
    }
    
    //% block="图像分类的结果,图片质量[QUALITY]" blockType="reporter"
    //% QUALITY.shadow="number" QUALITY.defl=50
    export function classifyImageRaw(parameter: any, block: any) {
        Generator.addInclude('local_classifier_include', '#include <LocalClassifier.h>');
        Generator.addObject('local_classifier_obj', 'LocalClassifier', 'classifier;');
        let quality = parameter.QUALITY.code;
        Generator.addCode([`classifier.classifyImageRaw(${quality})`, Generator.ORDER_UNARY_POSTFIX]);
    }
    //% block="百度OCR,图片质量[QUALITY]" blockType="reporter"
    //% QUALITY.shadow="number" QUALITY.defl=50
    export function baiduOcrRaw(parameter: any, block: any) {
        Generator.addInclude('local_classifier_include', '#include <LocalClassifier.h>');
        Generator.addObject('local_classifier_obj', 'LocalClassifier', 'classifier;');
        let quality = parameter.QUALITY.code;
        Generator.addCode([`classifier.baiduOcrRaw(${quality})`, Generator.ORDER_UNARY_POSTFIX]);
    }
 
}
