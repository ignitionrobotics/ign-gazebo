import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import IgnGazebo 1.0 as IgnGazebo

Rectangle {
  id: componentInspector
  color: "transparent"
  Layout.minimumWidth: 250
  Layout.minimumHeight: 375
  anchors.fill: parent

  /**
   * Time delay for tooltip to show, in ms
   */
  property int tooltipDelay: 500

  /**
   * Height of each item in pixels
   */
  property int itemHeight: 30

  /**
   * Color for even-numbered rows, according to current theme
   */
  property color even: (Material.theme == Material.Light) ?
    Material.color(Material.Grey, Material.Shade100) :
    Material.color(Material.Grey, Material.Shade800)

  /**
   * Color for odd-numbered rows, according to current theme
   */
  property color odd: (Material.theme == Material.Light) ?
    Material.color(Material.Grey, Material.Shade200) :
    Material.color(Material.Grey, Material.Shade900)

  function textFromModel(_model) {
    if (_model === null)
      return ''

    if (_model.typeName !== undefined)
      return _model.typeName

    if (_model.name !== undefined)
      return _model.name

    if (_model.x !== undefined)
      return (_model.x + " " +
              _model.y + " " +
              _model.z + " " +
              _model.roll + " " +
              _model.pitch + " " +
              _model.yaw)
  }

  Label {
    id: entityLabel
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    text: 'Entity: ' + ComponentInspector.entity
    font.pointSize: 13
    padding: 3
    background: Rectangle {
      color: even
    }
  }

  TreeView {
    id: tree
    frameVisible: false
    anchors.top: entityLabel.bottom
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    model: ComponentInspectorModel

    // Hacky: the sibling of listView is the background(Rectangle) of TreeView
    Component.onCompleted: {
      tree.__listView.parent.children[1].color = Material.background
    }
    Material.onThemeChanged: {
      tree.__listView.parent.children[1].color = Material.background
    }

    style: TreeViewStyle {
      indentation: itemHeight * 0.75

      headerDelegate: Rectangle {
        visible: false
      }

      branchDelegate: Rectangle {
        height: itemHeight
        width: itemHeight * 0.75
        color: "transparent"
        Image {
          id: icon
          sourceSize.height: itemHeight * 0.4
          sourceSize.width: itemHeight * 0.4
          fillMode: Image.Pad
          anchors.verticalCenter: parent.verticalCenter
          anchors.right: parent.right
          source: styleData.isExpanded ?
              "qrc:/Gazebo/images/minus.png" : "qrc:/Gazebo/images/plus.png"
        }
      }

      rowDelegate: Rectangle {
        visible: styleData.row !== undefined
        height: itemHeight
        color: (styleData.row % 2 == 0) ? even : odd
      }

      itemDelegate: Rectangle {
        id: itemDel
        color: (styleData.row % 2 == 0) ? even : odd
        height: itemHeight

        Text {
          anchors.verticalCenter: parent.verticalCenter
          leftPadding: 2
          text: textFromModel(model)
          color: Material.theme == Material.Light ? "black" : "white"
          font.pointSize: 12

          ToolTip {
            visible: ma.containsMouse
            delay: tooltipDelay
            text: model === null || model.typeId === undefined ?
                "Type Id: ?" : "Type Id: " + model.typeId
            y: itemDel.z - 30
            enter: null
            exit: null
          }
          MouseArea {
            id: ma
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.RightButton
            onClicked: {
              var type = EntityTreeModel.EntityType(styleData.index)
              var scopedName = EntityTreeModel.ScopedName(styleData.index)
              entityContextMenu.open(scopedName, type)
            }
          }
        }
      }
    }

    TableViewColumn {
      role: "typeName"
      width: 300
    }
  }

  IgnGazebo.EntityContextMenu {
    id: entityContextMenu
    anchors.fill: parent
  }
}
