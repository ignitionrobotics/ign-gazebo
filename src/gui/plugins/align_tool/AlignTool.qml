/*
 * Copyright (C) 2020 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
import QtQuick 2.9
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.2
import QtQuick.Controls.Material.impl 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

ToolBar {
  Layout.minimumWidth: 200
  Layout.minimumHeight: 370

  background: Rectangle {
    color: "transparent"
  }

  ButtonGroup {
    id: group
  }

  GridLayout {
    anchors.fill: parent
    columns: 4
    columnSpacing: 5
    rowSpacing: 5
    Text {
      text: "X:"
      Layout.row: 0
      Layout.column: 0
      font.weight: Font.Bold
      Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
      Layout.leftMargin: 25
      Layout.topMargin: 15
      font.pointSize: 14
    }
    /*
    ToolButton {
      id: x_min
      checkable: true
      ButtonGroup.group: group
      ToolTip.text: "View from the top"
      ToolTip.visible: hovered
      ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
      Layout.topMargin: 15
      Layout.leftMargin: 10
      contentItem: Image {
        fillMode: Image.Pad
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
        source: "x_min.png"
        sourceSize.width: 28;
        sourceSize.height: 28;
      }
      MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
          // TODO: hover object to new location
        }
        onExited: {
          // TODO: move object back to old location if not confirmed
        }
        onClicked: {
          AlignTool.OnAlignAxis("X")
          AlignTool.OnAlignConfig("min")
        }
      }
    }
    */
    ToolButton {
      id: x_center
      Layout.row: 0
      Layout.column: 1
      checkable: true
      ButtonGroup.group: group
      ToolTip.text: "Reset View Angle"
      ToolTip.visible: hovered
      ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
      Layout.topMargin: 15
      contentItem: Image {
        fillMode: Image.Pad
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
        source: "x_center.png"
        sourceSize.width: 28;
        sourceSize.height: 28;
      }
      MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
          AlignTool.OnAlignAxis("X")
          AlignTool.OnAlignConfig("center")
          AlignTool.OnHoveredEntered()
        }
        onExited: {
          AlignTool.OnHoveredExited()
        }
        onClicked: {
          AlignTool.SetAlignStatus("align")
        }
      }
    }
    /*
    ToolButton {
      id: x_max
      checkable: true
      ButtonGroup.group: group
      ToolTip.text: "View from the left"
      ToolTip.visible: hovered
      ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
      Layout.topMargin: 15
      contentItem: Image {
        fillMode: Image.Pad
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
        source: "x_max.png"
        sourceSize.width: 28;
        sourceSize.height: 28;
      }
      MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
          // TODO: hover object to new location
        }
        onExited: {
          // TODO: move object back to old location if not confirmed
        }
        onClicked: {
          AlignTool.OnAlignAxis("X")
          AlignTool.OnAlignConfig("max")
        }
      }
    }
    */
    Text {
      text: "Y:"
      font.weight: Font.Bold
      Layout.row: 1
      Layout.column: 0
      Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
      Layout.leftMargin: 25
      font.pointSize: 14
    }
    /*
    ToolButton {
      id: y_min
      checkable: true
      ButtonGroup.group: group
      ToolTip.text: "View from the front"
      ToolTip.visible: hovered
      ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
      Layout.leftMargin: 10
      contentItem: Image {
        fillMode: Image.Pad
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
        source: "y_min.png"
        sourceSize.width: 28;
        sourceSize.height: 28;
      }
      MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
          // TODO: hover object to new location
        }
        onExited: {
          // TODO: move object back to old location if not confirmed
        }
        onClicked: {
          AlignTool.OnAlignAxis("Y")
          AlignTool.OnAlignConfig("min")
        }
      }
    }
    */
    ToolButton {
      id: y_center
      checkable: true
      ButtonGroup.group: group
      Layout.row: 1
      Layout.column: 1
      ToolTip.text: "View from the right"
      ToolTip.visible: hovered
      ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
      contentItem: Image {
        fillMode: Image.Pad
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
        source: "y_center.png"
        sourceSize.width: 28;
        sourceSize.height: 28;
      }
      MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
          AlignTool.OnAlignAxis("Y")
          AlignTool.OnAlignConfig("center")
          AlignTool.OnHoveredEntered()
        }
        onExited: {
          AlignTool.OnHoveredExited()
        }
        onClicked: {
          AlignTool.SetAlignStatus("align")
        }
      }
    }
    /*
    ToolButton {
      id: y_max
      checkable: true
      ButtonGroup.group: group
      ToolTip.text: "View from the back"
      ToolTip.visible: hovered
      ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
      contentItem: Image {
        fillMode: Image.Pad
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
        source: "y_max.png"
        sourceSize.width: 28;
        sourceSize.height: 28;
      }
      MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
          // TODO: hover object to new location
        }
        onExited: {
          // TODO: move object back to old location if not confirmed
        }
        onClicked: {
          AlignTool.OnAlignAxis("Y")
          AlignTool.OnAlignConfig("max")
        }
      }
    }
    */
    Text {
      text: "Z:"
      font.weight: Font.Bold
      Layout.row: 2
      Layout.column: 0
      Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
      Layout.leftMargin: 25
      font.pointSize: 14
    }
    /*
    ToolButton {
      id: z_min
      checkable: true
      ButtonGroup.group: group
      ToolTip.text: "View from the bottom"
      ToolTip.visible: hovered
      ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
      Layout.leftMargin: 10
      contentItem: Image {
        fillMode: Image.Pad
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
        source: "z_min.png"
        sourceSize.width: 28;
        sourceSize.height: 28;
      }
      MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
          // TODO: hover object to new location
        }
        onExited: {
          // TODO: move object back to old location if not confirmed
        }
        onClicked: {
          AlignTool.OnAlignAxis("Z")
          AlignTool.OnAlignConfig("min")
        }
      }
    }
    */
    ToolButton {
      id: z_center
      checkable: true
      ButtonGroup.group: group
      ToolTip.text: "View from the bottom"
      ToolTip.visible: hovered
      ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
      Layout.row: 2
      Layout.column: 1
      contentItem: Image {
        fillMode: Image.Pad
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
        source: "z_center.png"
        sourceSize.width: 28;
        sourceSize.height: 28;
      }
      MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
          AlignTool.OnAlignAxis("Z")
          AlignTool.OnAlignConfig("center")
          AlignTool.OnHoveredEntered()
        }
        onExited: {
          AlignTool.OnHoveredExited()
        }
        onClicked: {
          AlignTool.SetAlignStatus("align")
        }
      }
    }
    /*
    ToolButton {
      id: z_max
      checkable: true
      ButtonGroup.group: group
      ToolTip.text: "View from the bottom"
      ToolTip.visible: hovered
      ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
      contentItem: Image {
        fillMode: Image.Pad
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
        source: "z_max.png"
        sourceSize.width: 28;
        sourceSize.height: 28;
      }
      MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
          // TODO: hover object to new location
        }
        onExited: {
          // TODO: move object back to old location if not confirmed
        }
        onClicked: {
          AlignTool.OnAlignAxis("Z")
          AlignTool.OnAlignConfig("max")
        }
      }
    }
    */
    CheckBox {
      text: qsTr("Reverse")
      Layout.columnSpan: 4
      Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
      Layout.leftMargin: 10
      checked: false
      onClicked: {
        AlignTool.OnReverse(checked)
      }
    }
    RowLayout {
      Layout.columnSpan: 4
      spacing: 10
      Text {
        text: "Relative to:"
        font.weight: Font.Bold
        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
        Layout.leftMargin: 15
        Layout.bottomMargin: 5
      }
      ComboBox {
        currentIndex: 0
        width: 1
        Layout.leftMargin: 10
        Layout.bottomMargin: 5
        model: ListModel {
          id: relativeAlignList
          ListElement { text: "First" }
          ListElement { text: "Last" }
        } 
        onCurrentIndexChanged: AlignTool.OnAlignTarget(relativeAlignList.get(currentIndex).text)
      }
    }
    Text {
      Layout.columnSpan: 4
      text: "Remember to Pause"
      font.weight: Font.Bold
      Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
    }
  }
}
