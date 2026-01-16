enum OCR {
    //% block="本地OCR"
    localOcr,
    //% block="百度OCR"
    baiduOcr,
}

//% color="#357DFF" iconWidth=50 iconHeight=40
namespace ClassifierApp {
//% block="本地分类推理" blockType="tag"
    export function localClassifier() {}

    
    //% block="初始化本地分类器 地址[URL]" blockType="command"
    //% URL.shadow="normal" URL.defl="192.168.1.100"
    export function initClassifier(parameter: any, block: any) {
        let url = parameter.URL.code;
        url = "http://" + url + ":8000";
        Generator.addInclude('local_classifier_include', '#include <LocalClassifier.h>');
        Generator.addObject('local_classifier_obj', 'LocalClassifier', 'classifier;');
        Generator.addSetup('local_classifier_init', `classifier.setBaseUrl("${url}");`);
    }

    //% block="对[TEXT]进行文本分类" blockType="reporter"
    //% TEXT.shadow="string" TEXT.defl="文本内容"
    export function classifyText(parameter: any, block: any) {
        let text = parameter.TEXT.code;
        Generator.addInclude('local_classifier_include', '#include <LocalClassifier.h>');
        Generator.addObject('local_classifier_obj', 'LocalClassifier', 'classifier;');
        Generator.addCode([`classifier.classifyText(${text})`, Generator.ORDER_UNARY_POSTFIX]);
    }
    
    //% block="拍照并进行图像分类,图片质量[QUALITY]" blockType="reporter"
    //% QUALITY.shadow="number" QUALITY.defl=50
    export function classifyImageRaw(parameter: any, block: any) {
        Generator.addInclude('local_classifier_include', '#include <LocalClassifier.h>');
        Generator.addObject('local_classifier_obj', 'LocalClassifier', 'classifier;');
        let quality = parameter.QUALITY.code;
        Generator.addCode([`classifier.classifyImageRaw(${quality})`, Generator.ORDER_UNARY_POSTFIX]);
    }
    //% block="使用[OCR]识别文字,图片质量[QUALITY]" blockType="reporter"
    //% OCR.shadow="dropdownRound" OCR.options="OCR" OCR.defl="本地OCR"
    //% QUALITY.shadow="number" QUALITY.defl=50
    export function baiduOcrRaw(parameter: any, block: any) {
        Generator.addInclude('local_classifier_include', '#include <LocalClassifier.h>');
        Generator.addObject('local_classifier_obj', 'LocalClassifier', 'classifier;');
        let quality = parameter.QUALITY.code;
        let ocr = parameter.OCR.code;
        if (ocr == "localOcr") {
            Generator.addCode([`classifier.localOcrRaw(${quality},0)`, Generator.ORDER_UNARY_POSTFIX]);
        } else {
            Generator.addCode([`classifier.baiduOcrRaw(${quality})`, Generator.ORDER_UNARY_POSTFIX]);
        }
    }

    //% block="中文字符处理" blockType="tag"
    export function Characterstag() {}

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
    
}
