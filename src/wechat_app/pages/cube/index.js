import * as THREE from '../../libs/three.weapp.js'
import { OrbitControls } from '../../jsm/controls/OrbitControls.js'
import gLTF from '../../jsm/loaders/GLTFLoader.js'
let GLTFLoader = gLTF(THREE)

Page({
  data: {
    canvasId: null
  },
  onLoad: function () {
    wx.createSelectorQuery()
      .select('#c')
      .node()
      .exec((res) => {
        let canvasId = res[0].node._canvasId
        const canvas = THREE.global.registerCanvas(canvasId, res[0].node)
        this.setData({ canvasId })

        // scene
        let scene = new THREE.Scene();

        // camera
        // PerspectiveCamera( fov : Number, aspect : Number, near : Number, far : Number )
        let camera = new THREE.PerspectiveCamera(90, canvas.width / canvas.height, 1, 1000);
        camera.position.set(30, 30, 30);  //( , z, )

        // render
        var renderer = new THREE.WebGLRenderer({
          antialias: true  // anti-alias 抗锯齿
        });
        renderer.physicallyCorrectLights = true;
        renderer.outputEncoding = THREE.sRGBEncoding;
        renderer.setClearColor( 0xcccccc );
        renderer.setSize(canvas.width, canvas.height);
        renderer.toneMapping = Number(THREE.LinearToneMapping);
        // 曝光 设置太大会导致模型难以分辨
        // https://gltf-viewer.donmccurdy.com
        renderer.toneMappingExposure = 2;

        // 场景背景色
        scene.background = new THREE.Color(0x444444);

        // load 3d model
        const loader = new GLTFLoader();
        // use Blender to export glb
        loader.load( '/pages/cube/xiao_assemble.glb', function ( gltf ) {
          scene.add( gltf.scene );
        }, undefined, function ( error ) {
          console.error( error );
        } );

        // 环境光
        // 0xffffff - white light, 0.5 - light intensity
        const ambientLight = new THREE.AmbientLight(0xffffff,1);
        scene.add(ambientLight);
        // 方向光源
        const directionalLight = new THREE.DirectionalLight(0xffffff, 0.3);
        directionalLight.position.set(4, 8, 4);
        scene.add(directionalLight);
        const directionalLight2 = new THREE.DirectionalLight(0xffffff, 0.3);
        directionalLight.position.set(0, -8, 0);
        scene.add(directionalLight2);
        // 用于显示光源方向，调试使用
        // let dhelper = new THREE.DirectionalLightHelper(directionalLight, 5, 0xff0000); 
        // scene.add(dhelper);

        // controls
        const controls = new OrbitControls(camera, renderer.domElement);
        controls.enableZoom = false;  // 禁用缩放
        controls.update();

        function onWindowResize() {
          camera.aspect = window.innerWidth / window.innerHeight;
          camera.updateProjectionMatrix();
          renderer.setSize(canvas.width, canvas.height);
        }
        function render() {
          canvas.requestAnimationFrame(render);

          // calculate mesh oritation from acc
          // const app = getApp()
          // mesh.rotation.z = -1 * Math.atan(app.globalData.parsedIMUData.acc_x/app.globalData.parsedIMUData.acc_z)
          // mesh.rotation.x = -1 * Math.atan(app.globalData.parsedIMUData.acc_y/app.globalData.parsedIMUData.acc_z)

          // gltf.rotation.x += 0.005;
          // gltf.rotation.y += 0.01;
          controls.update();
          renderer.render(scene, camera);
        }
        render()
      })
  },
  onUnload: function () {
    THREE.global.unregisterCanvas(this.data.canvasId)
  },
  touchStart(e) {
    console.log('canvas', e)
    THREE.global.touchEventHandlerFactory('canvas', 'touchstart')(e)
  },
  touchMove(e) {
    console.log('canvas', e)
    THREE.global.touchEventHandlerFactory('canvas', 'touchmove')(e)
  },
  touchEnd(e) {
    console.log('canvas', e)
    THREE.global.touchEventHandlerFactory('canvas', 'touchend')(e)
  },
  touchCancel(e) {
    // console.log('canvas', e)
  },
  longTap(e) {
    // console.log('canvas', e)
  },
  tap(e) {
    // console.log('canvas', e)
  },
  documentTouchStart(e) {
    // console.log('document',e)
  },
  documentTouchMove(e) {
    // console.log('document',e)
  },
  documentTouchEnd(e) {
    // console.log('document',e)
  },
})
