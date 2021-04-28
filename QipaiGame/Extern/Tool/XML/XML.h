#ifndef _XML_H__
#define _XML_H__

#include "CommonHead.h"

#ifdef _DEBUG
#define _DS(i) (i?&(m_strDoc.c_str())[m_aPos[i].nStartL]:0)
#define MARKUP_SETDEBUGSTATE m_pMainDS=_DS(m_iPos); m_pChildDS=_DS(m_iPosChild)
#else
#define MARKUP_SETDEBUGSTATE
#endif


class XML
{
public:
    XML() { SetDoc( NULL ); };
    XML( const char* szDoc ) { SetDoc( szDoc ); };
    XML( const XML& markup ) { *this = markup; };
    void operator=( const XML& markup );
    virtual ~XML() {};

    // Navigate
    bool Load( const char* szFileName );
    bool SetDoc( const char* szDoc );
    bool IsWellFormed();
    bool FindElem( const char* szName=NULL );
    bool FindChildElem( const char* szName=NULL );
    bool IntoElem();
    bool OutOfElem();
    void ResetChildPos() { x_SetPos(m_iPosParent,m_iPos,0); };
    void ResetMainPos() { x_SetPos(m_iPosParent,0,0); };
    void ResetPos() { x_SetPos(0,0,0); };
    std::string GetTagName() const;
    std::string GetChildTagName() const { return x_GetTagName(m_iPosChild); };
    std::string GetData() const { return x_GetData(m_iPos); };
    std::string GetChildData() const { return x_GetData(m_iPosChild); };
    //[CMARKUPDEV
    std::string FindGetData( const char* szName );
    //]CMARKUPDEV
    std::string GetAttrib( const char* szAttrib ) const { return x_GetAttrib(m_iPos,szAttrib); };
    std::string GetChildAttrib( const char* szAttrib ) const { return x_GetAttrib(m_iPosChild,szAttrib); };
    std::string GetAttribName( int n ) const;
    bool SavePos( const char* szPosName="" );
    bool RestorePos( const char* szPosName="" );
    bool GetOffsets( int& nStart, int& nEnd ) const;
    std::string GetError() const { return m_strError; };

    enum MarkupNodeType
    {
        MNT_ELEMENT					= 1,  // 0x01
        MNT_TEXT					= 2,  // 0x02
        MNT_WHITESPACE				= 4,  // 0x04
        MNT_CDATA_SECTION			= 8,  // 0x08
        MNT_PROCESSING_INSTRUCTION	= 16, // 0x10
        MNT_COMMENT					= 32, // 0x20
        MNT_DOCUMENT_TYPE			= 64, // 0x40
        MNT_EXCLUDE_WHITESPACE		= 123,// 0x7b
    };
    //[CMARKUPDEV
    int FindNode( int nType=0 );
    int GetNodeType() { return m_nNodeType; };
    bool AddNode( int nType, const char* szText ) { return x_AddNode(nType,szText,false); };
    bool InsertNode( int nType, const char* szText ) { return x_AddNode(nType,szText,true); };
    bool RemoveNode();
    //]CMARKUPDEV

    // create
    bool Save( const char* szFileName );
    std::string GetDoc() const { return m_strDoc; };
    bool AddElem( const char* szName, const char* szData=NULL ) { return x_AddElem(szName,szData,false,false); };
    bool InsertElem( const char* szName, const char* szData=NULL ) { return x_AddElem(szName,szData,true,false); };
    bool AddChildElem( const char* szName, const char* szData=NULL ) { return x_AddElem(szName,szData,false,true); };
    bool InsertChildElem( const char* szName, const char* szData=NULL ) { return x_AddElem(szName,szData,true,true); };
    bool AddAttrib( const char* szAttrib, const char* szValue ) { return x_SetAttrib(m_iPos,szAttrib,szValue); };
    bool AddChildAttrib( const char* szAttrib, const char* szValue ) { return x_SetAttrib(m_iPosChild,szAttrib,szValue); };
    bool AddAttrib( const char* szAttrib, int nValue ) { return x_SetAttrib(m_iPos,szAttrib,nValue); };
    bool AddChildAttrib( const char* szAttrib, int nValue ) { return x_SetAttrib(m_iPosChild,szAttrib,nValue); };
    bool AddChildSubDoc( const char* szSubDoc ) { return x_AddSubDoc(szSubDoc,false,true); };
    bool InsertChildSubDoc( const char* szSubDoc ) { return x_AddSubDoc(szSubDoc,true,true); };
    std::string GetChildSubDoc() const;

    // Modify
    bool RemoveElem();
    bool RemoveChildElem();
    bool SetAttrib( const char* szAttrib, const char* szValue ) { return x_SetAttrib(m_iPos,szAttrib,szValue); };
    bool SetChildAttrib( const char* szAttrib, const char* szValue ) { return x_SetAttrib(m_iPosChild,szAttrib,szValue); };
    bool SetAttrib( const char* szAttrib, int nValue ) { return x_SetAttrib(m_iPos,szAttrib,nValue); };
    bool SetChildAttrib( const char* szAttrib, int nValue ) { return x_SetAttrib(m_iPosChild,szAttrib,nValue); };
    //[CMARKUPDEV
    bool RemoveAttrib( const char* szAttrib ) { return x_RemoveAttrib(m_iPos,szAttrib); };
    bool RemoveChildAttrib( const char* szAttrib ) { return x_RemoveAttrib(m_iPosChild,szAttrib); };
    bool FindSetData( const char* szName, const char* szData, int nCDATA=0 );
    //]CMARKUPDEV
    bool SetData( const char* szData, int nCDATA=0 ) { return x_SetData(m_iPos,szData,nCDATA); };
    bool SetChildData( const char* szData, int nCDATA=0 ) { return x_SetData(m_iPosChild,szData,nCDATA); };

    //[CMARKUPDEV
    // Base64
    static std::string EncodeBase64( const unsigned char* pBuffer, int nBufferLen );
    static int DecodeBase64( const std::string& strBase64, unsigned char* pBuffer, int nBufferLen );
    //]CMARKUPDEV

protected:

#ifdef _DEBUG
    const char* m_pMainDS;
    const char* m_pChildDS;
#endif

    std::string m_strDoc;
    std::string m_strError;

    struct ElemPos
    {
        ElemPos() { Clear(); };
        ElemPos( const ElemPos& pos ) { *this = pos; };
        bool IsEmptyElement() const { return (nStartR == nEndL + 1); };
        void Clear()
        {
            nStartL=0; nStartR=0; nEndL=0; nEndR=0; nReserved=0;
            iElemParent=0; iElemChild=0; iElemNext=0;
        };
        void AdjustStart( int n ) { nStartL+=n; nStartR+=n; };
        void AdjustEnd( int n ) { nEndL+=n; nEndR+=n; };
        int nStartL;
        int nStartR;
        int nEndL;
        int nEndR;
        int nReserved;
        int iElemParent;
        int iElemChild;
        int iElemNext;
    };

    typedef std::vector<ElemPos> vectorElemPosT;
    vectorElemPosT m_aPos;
    int m_iPosParent;
    int m_iPos;
    int m_iPosChild;
    int m_iPosFree;
    int m_nNodeType;
    //[CMARKUPDEV
    int m_nNodeOffset;
    int m_nNodeLength;
    //]CMARKUPDEV

    struct TokenPos
    {
        TokenPos( const char* sz ) { Clear(); szDoc = sz; };
        void Clear() { nL=0; nR=-1; nNext=0; bIsString=false; };
        bool Match( const char* szName )
        {
            int nLen = nR - nL + 1;
            // To ignore case, define MARKUP_IGNORECASE
#ifdef MARKUP_IGNORECASE
            return ( (strnicmp( &szDoc[nL], szName, nLen ) == 0)
#else
            return ( (strncmp( &szDoc[nL], szName, nLen ) == 0)
#endif
                && ( szName[nLen] == '\0' || strchr(" =/[",szName[nLen]) ) );
        };
        int nL;
        int nR;
        int nNext;
        const char* szDoc;
        bool bIsString;
    };

    struct SavedPos
    {
        int iPosParent;
        int iPos;
        int iPosChild;
    };
    typedef std::map<std::string,SavedPos> mapSavedPosT;
    mapSavedPosT m_mapSavedPos;

    void x_SetPos( int iPosParent, int iPos, int iPosChild )
    {
        m_iPosParent = iPosParent;
        m_iPos = iPos;
        m_iPosChild = iPosChild;
        m_nNodeType = iPos?MNT_ELEMENT:0;
        //[CMARKUPDEV
        m_nNodeOffset = 0;
        m_nNodeLength = 0;
        //]CMARKUPDEV
        MARKUP_SETDEBUGSTATE;
    };

    int x_GetFreePos();
    int x_ReleasePos();

    int x_ParseElem( int iPos );
    int x_ParseError( const char* szError, const char* szName = NULL );
    static bool x_FindChar( const char* szDoc, int& nChar, char c );
    static bool x_FindToken( TokenPos& token );
    std::string x_GetToken( const TokenPos& token ) const;
    int x_FindElem( int iPosParent, int iPos, const char* szPath );
    std::string x_GetTagName( int iPos ) const;
    std::string x_GetData( int iPos ) const;
    std::string x_GetAttrib( int iPos, const char* szAttrib ) const;
    bool x_AddElem( const char* szName, const char* szValue, bool bInsert, bool bAddChild );
    bool x_AddSubDoc( const char* szSubDoc, bool bInsert, bool bAddChild );
    bool x_FindAttrib( TokenPos& token, const char* szAttrib=NULL ) const;
    bool x_SetAttrib( int iPos, const char* szAttrib, const char* szValue );
    bool x_SetAttrib( int iPos, const char* szAttrib, int nValue );
    //[CMARKUPDEV
    bool x_RemoveAttrib( int iPos, const char* szAttrib );
    bool x_AddNode( int nNodeType, const char* szText, bool bInsert );
    void x_RemoveNode( int iPosParent, int& iPos, int& nNodeType, int& nNodeOffset, int& nNodeLength );
    void x_AdjustForNode( int iPosParent, int iPos, int nShift );
    //]CMARKUPDEV
    bool x_CreateNode( std::string& strNode, int nNodeType, const char* szText );
    void x_LocateNew( int iPosParent, int& iPosRel, int& nOffset, int nLength, int nFlags );
    int x_ParseNode( TokenPos& token );
    bool x_SetData( int iPos, const char* szData, int nCDATA );
    int x_RemoveElem( int iPos );
    void x_DocChange( int nLeft, int nReplace, const std::string& strInsert );
    void x_PosInsert( int iPos, int nInsertLength );
    void x_Adjust( int iPos, int nShift, bool bAfterPos = false );
    std::string x_TextToDoc( const char* szText, bool bAttrib = false ) const;
    std::string x_TextFromDoc( int nLeft, int nRight ) const;
};



/*

Load 导入一个XML文件到CMarkup的对象中，并对它进行解析。类似C#的Load。
SetDoc 从字符串中导入XML数据，并对它解析。类似C#的LoadXml
Save 将XML数据写入文件中。类似C#的Save。 
GetDoc 将整个XML数据文档作为字符串返回。
FindElem 定位到下一个元素，可能和一个标签名或路径匹配。 
FindChildElem 定位到下一个子元素，匹配元素名或路径。  
FindPrevElem 定位前一个元素，可能和一个标签名或路径匹配。 
FindPrevChildElem 定位前一个子元素，可能匹配标签名。 
FindNode  定位下一个节点，可能和节点类型匹配。  
IntoElem  进入当前主位置的下一级，当前的位置变为父位置。
OutOfElem 使当前父位置变成当前位置。 
ResetPos 复位当前位置为文档起始位置。 
ResetMainPos 将当前主位置复位为第一个兄弟位置之前。
ResetChildPos 复位当前子位置到第一个子位置之前
AddElem 在当前主位置元素或最后兄弟位置之后增加一个元素。 
InsertElem 在当前主位置元素或第一个兄弟位置之前插入一个元素。 
AddChildElem 在当前子位置元素或最后一个子位置之后增加一个元素。 
InsertChileElem 在当前子位置元素或低一个子位置之前插入一个元素。 
AddSubDoc 在当前主位置元素或最后一个兄弟位置之后增加一个子文档。 
InsertSubDoc 在当前主位置元素或第一个兄弟位置之前插入一个子文档。 
AddChildSubDoc 在当前子位置元素或最后一个子位置之后增加子文档。 
InsertChildSubDoc 在当前子位置元素或第一个子位置之前插入一个子文档。 
AddNode 在当前节点之后或父元素内容末尾增加一个节点。 
InsertNode 在当前节点之前或父元素内容开头插入一个节点
RemoveElem 删除当前包括子元素的主位置元素 
RemoveChildElem 删除包括当前子元素及其子元素 
RemoveNode 删除当前节点  
RemoveAttrib 删除当前位置元素具体的属性
RemoveChildAttrib 删除当前子位置元素的某个具体属性
GetData 得到当前主位置元素或节点的字符串值 
GetChildData 得到当前子位置元素的字符串值  
GetElemContent 得到当前主位置元素包括其子元素的标记内容字符串值 
GetSubDoc 得到当前主位置元素包括其子元素的文档片断标记字符串值  
GetChildSubDoc 得到当前子位置元素包括其子元素的文档片断标记字符串值 
GetAttrib 得到主位置元素（或正在进行的指令的）某一具体属性字符串值 
GetChildAttrib 得到子位置某一特定属性的字符串值  
GetTagName 得到主位置元素（或正在进行的指令的）标签名称 
GetChildTagName 得到子位置元素的标签名称  
FindGetData 定位到匹配某一具体路径的下一个元素并返回字符串值
SetData 设置当前主位置元素或节点的值 
SetChildData 设置当前子位置元素的值  
SetElemContent 设置当前主位置元素的标记内容  
SetAttrib 设置当前主位置元素（或正在进行的指令的）某一具体属性的值 
SetChildAttrib 设置当前子位置元素某一具体属性的值  
FindSetData 定位匹配某一具体路径的下一个元素并设置其值 

GetAttribName 当过当前位置元素属性的具体索引得到属性名称 
GetNodeType 得到当前节点的节点类型 
GetElemLevel 得到当前主位置的级数 
GetElemFlags 得到当前主位置元素的标志 
SetElemFlags 设置当前主位置元素的标志 
GetOffsets 获得在当前主位置偏移的文档文本  
GetAttribOffsets 获得在当前主位置特定属性便宜的文档文本
SavePos 在hash map中使用可选字符串名称保存当前位置 
RestorePos 定位到通过SavePos保存的位置  
SetMapSize 设置SavePos和RestorePos使用的hash map大小 
GetElemIndex 得到当前主位置元素整形索引值  
GotoElemIndex 设置当前主位置元素为给定的整形索引值 
GetChildElemIndex 得到当前子位置元素的整形索引值  
GotoChildElemIndex 设置当前子位置元素为给定的整形索引值 
GetParentElemIndex 获得当前父位置元素的整形索引值  
GotoParentElemIndex 设置当前父位置元素为给定的整形索引值 
GetElemPath 获得表示主位置元素绝对路径的字符串  
GetChildElemPath 获得表示子位置元素的绝对路径的字符串 
GetParentElemPath 获得表示父位置元素的绝对路径的字符串
IsWellFormed 判定文档是否有单一根元素和恰当地包含元素 GetError 从最后的解析中返回错误（信息）字符串 GetDocFlags 返回文档标志
SetDocFlags 设置文档标志  
GetDocElemCount 返回文档中元素的个数

ReadTextFile 读一个文本文件转成字符串 
WirteTextFile 写字符串到文本文件中  
GetDeclareEncoding 从XML声明中得到编码的名字 
EscapeText 返回标记中某一字符的编码  
UnescapeText 返回字符串值得某一特定字符解码  
UTF8ToA 将UTF-8字符转成非Unicode（如ANSI）字符 
AToUTF8 将非Unicode（如ANSI）字符转成UTF-8字符 
UTF16T08 将UTF-16字符转成UTF-8 
UTF8To16 将UTF-8字符转成UTF-16  
EncodeBase64 将二进制数据译成Base64字符串 
DecodeBase64 将Base64字符译成二进制数据
*/

#endif // !defined(_XML_H__)
