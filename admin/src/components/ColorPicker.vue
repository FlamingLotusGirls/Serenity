<template>
    <div class="input-group color-picker" ref="colorpicker">
        <input type="text" class="form-control" v-model="value" @focus="showPicker()" @input="updateFromInput" />
         <div class="input-group-append">
            <span class="input-group-text">
                <span class="current-color" :style="'background-color: ' + value" @click="togglePicker()"></span>
                <SwatchPicker :value="colors" @input="updateFromPicker" v-if="displayPicker" />
            </span>
        </div>
    </div>
</template>

<script>
// adapted from a component written by @brownsugar and posted on GitHub via CodePen
// GitHub Issues: https://github.com/xiaokaike/vue-color/issues/76#issuecomment-329956525
// CodePen: https://codepen.io/Brownsugar/pen/NaGPKy
import { Swatches } from 'vue-color';

export default {
	components: {
		'SwatchPicker': Swatches,
	},
	props: ['value'],
	data() {
		return {
			colors: {
				hex: '#ff0000',
			},
			displayPicker: false,
		}
	},
	methods: {
		updateColors(color) {
			if(color.slice(0, 1) == '#') {
				this.colors = {
					hex: color
				};
			}
			else if(color.slice(0, 4) == 'rgba') {
				var rgba = color.replace(/^rgba?\(|\s+|\)$/g,'').split(','),
					hex = '#' + ((1 << 24) + (parseInt(rgba[0]) << 16) + (parseInt(rgba[1]) << 8) + parseInt(rgba[2])).toString(16).slice(1);
				this.colors = {
					hex: hex,
					a: rgba[3],
				}
			}
		},
		showPicker() {
			document.addEventListener('click', this.documentClick);
			this.displayPicker = true;
		},
		hidePicker() {
			document.removeEventListener('click', this.documentClick);
			this.displayPicker = false;
		},
		togglePicker() {
			this.displayPicker ? this.hidePicker() : this.showPicker();
		},
		updateFromInput() {
			this.updateColors(this.value);
		},
		updateFromPicker(color) {
			this.colors = color;
			let newValue;
			if(color.rgba.a == 1) {
				newValue = color.hex;
			}
			else {
				newValue = 'rgba(' + color.rgba.r + ', ' + color.rgba.g + ', ' + color.rgba.b + ', ' + color.rgba.a + ')';
			}

			this.$emit('input', newValue);
		},
		documentClick(e) {
			var el = this.$refs.colorpicker,
				target = e.target;
			if(el !== target && !el.contains(target)) {
				this.hidePicker()
			}
		}
	},
	watch: {
		value(val) {
			if(val) {
				this.updateColors(val);
				this.$emit('input', val);
				//document.body.style.background = val;
			}
		}
	}
};
</script>

<style>
.color-picker {
	margin-bottom: 12px;
}
.vc-chrome, .vc-swatches {
	position: absolute;
	top: 35px;
	right: 0;
	z-index: 9;
}
.current-color {
	display: inline-block;
	width: 16px;
	height: 16px;
	background-color: #000;
	cursor: pointer;
}
</style>