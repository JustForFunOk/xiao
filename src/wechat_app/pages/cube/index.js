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
        // const camera = new THREE.PerspectiveCamera(70, canvas.width / canvas.height, 1, 1000);
        // camera.position.z = 500;

        // scene
        let scene = new THREE.Scene();
        scene.background = new THREE.Color(0xB3CEFB);
        scene.fog = new THREE.Fog(scene.background, 1, 100);

        // const scene = new THREE.Scene();
        // scene.background = new THREE.Color(0xAAAAAA);

        // camera
        let camera = new THREE.PerspectiveCamera(90, canvas.width / canvas.height, 1, 1000);
        camera.position.set(30, 30, 30);  //( , z, )

        // render
        let renderer = new THREE.WebGLRenderer({
          // alpha: true, 
          antialias: true
        });
        renderer.setSize(canvas.width, canvas.height);

        // load 3d model
        const loader = new GLTFLoader();
        loader.load( '/pages/cube/xiao_assemble.glb', function ( gltf ) {
          scene.add( gltf.scene );
          // 模型Mesh开启阴影
          gltf.scene.traverse(obj => {
            if(obj.isMesh) {
            obj.castShadow = true;
            obj.receiveShadow = true;
            // obj.material.emissiveMap = obj.material.map;
            const color = new THREE.Color(0xffff00);
            obj.color = color;
            }
          })
          
          
        }, undefined, function ( error ) {
          console.error( error );
        } );

        // light
        // 0xffffff - white light, 0.5 - light intensity
        let directionalLight = new THREE.DirectionalLight(0xffffff, 1.0);
        directionalLight.position.set(4, 8, 4);
        scene.add(directionalLight);
        // let dhelper = new THREE.DirectionalLightHelper(directionalLight, 5, 0xff0000); 
        // scene.add(dhelper);
        
        // let hemisphereLight = new THREE.HemisphereLight(0xffffff, 0xffffff, 0.4);
        // hemisphereLight.position.set(4, -8, 4);
        // scene.add(hemisphereLight);
        // let hHelper = new THREE.HemisphereLightHelper(hemisphereLight, 5);
        // scene.add(hHelper);

        // floor
        let floorGeometry = new THREE.PlaneGeometry(3000, 3000, 1);
        let floorMaterial = new THREE.MeshPhongMaterial({
          color: 0x77F28F,  // green
          shininess: 0,
          // wireframe: true
        });
        let floor = new THREE.Mesh(floorGeometry, floorMaterial);
        floor.rotation.x = -0.5 * Math.PI;
        floor.position.y = -20;
        scene.add(floor);

        // shadow
        // 首先渲染器开启阴影
        renderer.shadowMap.enabled = true;
        // 光源开启阴影
        directionalLight.castShadow = true;
        directionalLight.shadow.mapSize = new THREE.Vector2(1024, 1024);
        // 地板接受阴影开启
        floor.receiveShadow = true;

        
        // controls
        const controls = new OrbitControls(camera, renderer.domElement);
        // controls.enableDamping = true;
        // controls.dampingFactor = 0.25;
        // controls.enableZoom = false;
        controls.update();

        // const geometry = new THREE.BoxBufferGeometry(250, 100, 200);
        // const texture = new THREE.TextureLoader().load('./rainbow.jpg');
        // const material = new THREE.MeshBasicMaterial({ map: texture });
        // // const material = new THREE.MeshBasicMaterial({ color: 0x44aa88 });
        // const mesh = new THREE.Mesh(geometry, material);
        // scene.add(mesh);

        // renderer.setPixelRatio(wx.getSystemInfoSync().pixelRatio);
        // renderer.setSize(canvas.width, canvas.height);

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

          // mesh.rotation.x += 0.005;
          // mesh.rotation.y += 0.01;
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
