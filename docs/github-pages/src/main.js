import { createApp } from 'vue'
import { createRouter, createWebHistory } from 'vue-router'
import App from './App.vue'

// Import pages
import HomePage from './pages/HomePage.vue'
import GameDatabase from './pages/GameDatabase.vue'
import VRAnalysis from './pages/VRAnalysis.vue'
import AIInsights from './pages/AIInsights.vue'
import Documentation from './pages/Documentation.vue'

// Create router
const routes = [
  { path: '/', name: 'Home', component: HomePage },
  { path: '/database', name: 'Database', component: GameDatabase },
  { path: '/analysis', name: 'Analysis', component: VRAnalysis },
  { path: '/insights', name: 'Insights', component: AIInsights },
  { path: '/docs', name: 'Documentation', component: Documentation }
]

const router = createRouter({
  history: createWebHistory('/uevrLCX/'),
  routes
})

// Create app
const app = createApp(App)
app.use(router)
app.mount('#app')

// Add global error handling
window.addEventListener('error', (event) => {
  console.error('Global error:', event.error)
})

// Add performance monitoring
if ('performance' in window) {
  window.addEventListener('load', () => {
    const loadTime = performance.now()
    console.log(`Context7 loaded in ${loadTime.toFixed(2)}ms`)
  })
}