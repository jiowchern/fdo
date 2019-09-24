### 自我介紹 
我自小就熱愛遊戲開發，雖然已經快四十歲很多同事或朋友都已經脫離這個行業，但是我現在還是**無法想像我不再遊戲業的樣子**，所以我想我沒什大問題的話我應該會持續在這個行業服務...  
工作經歷請看我的 [Linkedin](https://www.linkedin.com/in/jiowchern-chen-4aaa90b7/)。



### 關於專業技能
**2004~2012**  
主要開發語言是 c++ ，這段時間我大致上服務於兩家公司，都是用 c++ 開發 mmorpg 專案，因為時間久遠我大致羅列比較有印象的任務。
1. 導入 mfc 訊息對應巨集架構。 改善專案獨體過多高度耦合的狀況。([部分代碼](https://github.com/jiowchern/fdo/blob/master/CoreSystem/EventSystem/GameMessageEvent.h))
2. 實作記憶體池( memory pool )，加速遊戲物件配置與釋放速度，更使用 typelist 技術將物件配置在編譯期就完成指定記憶體池的大小配置。節省記憶體池的空間。([部分代碼](https://github.com/jiowchern/fdo/blob/master/CoreSystem/EventSystem/GameMessageEventBase.h))
3. 於 c++ 裡實作類似 lambda 的架構使 non-blocking 類代碼更方便維護。(當時我還不知道 lambda 且尚未有 c++0x 。) ([部分代碼](https://github.com/jiowchern/fdo/blob/master/CoreSystem/Command/GameCommand.h))
4. 撰寫 ui 編輯器，縮短程式開發人員在介面排版上的開發時間。([部分代碼](https://github.com/jiowchern/fdo/tree/master/CoreSystem/GUI/Creator))
5. 實作圖文字系統，將文字繪製於材質，實現文字剪裁也提升專案幀數([部分代碼](https://github.com/jiowchern/fdo/tree/master/CoreSystem/FontSystem))

**2012~2019**  
主要開發語言 c# ，因為這段時間開始接觸 Unity 3.5 ，服務於幾家公司。  
這段時間我開始思考如何於服務器與用戶端共寫一份代碼的技術，因此開發了基於 **Remote method invocation** 的網路框架 ([link](https://github.com/jiowchern/Regulus))。這個框架優勢在於可以將服務器上的物件以介面形式與用戶端溝通大大的模組化服務器與用戶端之前資料傳遞的代碼。服務器之間也可以透過此方法傳遞物件不用再撰寫冗長的轉發封包。其中為了相容於 Aot 的限制自己實作了類 protobuf 序列化模組 ([link](https://github.com/jiowchern/Regulus/tree/master/Library/Regulus.Serialization))，之後實作 rudp 為了優化 tcp 慢啟動的問題 ([link](https://github.com/jiowchern/Regulus/tree/master/Library/Regulus.Network))。與此同時也嘗試用此框架試作線上動作遊戲專案([link](https://github.com/jiowchern/GameProject1))，在這專案中也有實現[行為樹](https://github.com/jiowchern/Regulus/tree/master/Library/RegulusBehaviourTree)邏輯應用於怪物 ai。

### 關於現況
雖然我也好幾年沒碰 C++ 當然也沒操作過 Unreal 對於程式語言我有自信能短時間更甚以往水平，至於遊戲引擎自己本身本來就對 Unreal 很有興趣往後日子也會開始自研，也希望公司能給我面試機會。






