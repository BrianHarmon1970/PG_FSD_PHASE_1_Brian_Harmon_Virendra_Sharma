object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Form1'
  ClientHeight = 109
  ClientWidth = 364
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 8
    Top = 8
    Width = 353
    Height = 97
    Caption = 'Panel1'
    TabOrder = 0
    object VolText0: TLabel
      Left = 319
      Top = 27
      Width = 12
      Height = 13
      Caption = '88'
    end
    object VolText1: TLabel
      Left = 319
      Top = 44
      Width = 12
      Height = 13
      Caption = '70'
    end
    object LevelControl0: TScrollBar
      Left = 0
      Top = 24
      Width = 313
      Height = 20
      PageSize = 0
      Position = 88
      TabOrder = 0
      OnChange = VC0Changed
    end
    object LevelControl1: TScrollBar
      Left = 0
      Top = 40
      Width = 313
      Height = 17
      PageSize = 0
      Position = 70
      TabOrder = 1
      StyleElements = [seFont, seClient]
      OnChange = VCChanged1
    end
    object DeviceList: TComboBox
      Left = 0
      Top = 0
      Width = 353
      Height = 21
      BevelKind = bkSoft
      TabOrder = 2
      Text = 'DeviceList'
      OnChange = DeviceSelected
      OnSelect = DeviceSelected
    end
    object LevelMeter0: TProgressBar
      Left = 0
      Top = 63
      Width = 353
      Height = 17
      BorderWidth = 1
      Position = 75
      BarColor = clYellow
      BackgroundColor = clBlue
      TabOrder = 3
    end
    object LevelMeter1: TProgressBar
      Left = 0
      Top = 80
      Width = 353
      Height = 17
      BorderWidth = 1
      Position = 34
      MarqueeInterval = 2
      BarColor = clYellow
      TabOrder = 4
    end
  end
  object Timer1: TTimer
    Interval = 10
    OnTimer = TimerTick
    Left = 176
    Top = 64
  end
end