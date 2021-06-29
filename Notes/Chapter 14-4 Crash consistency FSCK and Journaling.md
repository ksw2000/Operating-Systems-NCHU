# Crash Consistency FSCK and Journaling

> 當我們擴大一個檔案時要更改三個資訊
>
> 1. indoe：更新 meta data 並 allocate 新的 data block
> 2. data bitmap
> 3. data block

## Crash-Consistency Problem

但是... 如果這三個動作到一半時，系統當機

1. 若只寫了 data block

   > 對檔案系統來說這個 data block 是空的 → File system is consistent → But data lost

2. 若只寫了 indoe

   > indoe 上告訴我們某個 data block 有在使用，而 Data bitmap 則表示是 free
   >
   > 如果我們相信 indoe → 讀到 garbage data
   >
   > 我果我們相信 data bitmap → 沒問題
   >
   > File system inconsistent

3. 若只寫了 data bitmap

   > 同之前情況 indoe 和 data bitmap 資訊不一致
   >
   > 如果我們相信 indoe → space leak (默默的浪費硬碟空間，檔案系統不知道那筆資料主人是誰)
   >
   > File system inconsistent

4. 若只寫了 indoe 和 data block

   > indoe 有指向 data block ，但 data bitmap 說那是空的
   >
   > File system inconsistent

5. 若只寫了 indoe 和 data block

   > indoe 沒有指向 data block ，該筆資料無法使用 → space leak
   >
   > File system inconsistent

6. 若只寫了 indoe 和 data bitmap

   > 檔案多了垃圾資料但 File system consistent

## File Consistent Checker (事後治療)

fsck in Unix-based OS

Goal: make sure the file system metadata is consistently

1. 先檢查 super block

2. 從 indoe 尋找已宣告的空間 (allocation bitmap)，並跟 data bitmap 做比較

   > 若兩者不一致時，FSCK 選擇相信 indoe

3. 接著檢查 indoe bitmap，FSCK 會描瞄所有 indoe 產生 produce indoe bitmap 再與原本的 indoe bitmap 做比較

   > 若兩者不一致時，FSCK 選擇相信 indoe

Problem

1. Challenging to implement *fsck*
2. too slow (硬碟越做越大)

## Journaling (事先預防)

Journling aka write-ahead logging

Write-ahead: Before overwriting the data structures in place, first write down a little note describing what you are about to do.

Log: the notes are organized into a log

TxB: Transaction begin, TxE: Transaction end

![](./src/data-journal.png)

在我們寫檔案之前我們就先到 journaling 記錄我們所要執行的「動作」，我們稱整個過程為一個 transaction，完成 transcation 時也稱做 be committed

在 commit 之後 (當 journaling 完成後)，才去更動 file system，俟 file ssytem 確實執行動作後再將 journaling 移除，我們稱這系列動作為 checkpoint

**如果 crash 發生**

1. 如果 transaction 到一半失敗

   > 電腦重開時會發現 transaction 沒有完成，會將其移除，file system 仍是 consistent ，只是使用者資料會消失

2. 如果 check point 時當掉

   > 電腦重開時會發現 journal 裡怎麼還有 transaction，理論上來說做完的 jounal 會被清除，所以 OS 會重做一遍，完成後再將 jounaling 清除

**效能**

為了效能，有時會先將 journaling 先 buffer 在 memory 再寫進硬碟

### metadata journaling

Now, for each write to disk, we are writing each block to the disk twice.

data 要先寫進 journal 才會寫進 datablock，因此衍生出另一個 journaling, matadata journaling 只將 indoe, data bitmap 寫進 journal ，data block 則直接寫入

**Method 1: Journal write → data write → check point**

如果 jounal write 出事 → 重開機時重新運作 → 資料遺失 → consistent

如果 data write 出事 → *indoe 會指向 garbage data* → consistent

如果 check point 出事 → 重新運作 → consistent

**Method 2: data write → journal write → check point**

如果 data write 出事 → 資料遺失 → consistent

如果 journal write 出事 → 重開機時重新運作並取消動作 → 資料遺失 → consistent

如果 check point 出事 → 重新運作 → consistent

**結論**

因為 Method1 有可能會出現 indoe 指向 garbage block 的可能，所以現在大部分都是以 Method2 來實作

另外，為了與 metadata journaling 做區別，之前一起把 data 寫進 jounal 的方法稱為 data journaling

